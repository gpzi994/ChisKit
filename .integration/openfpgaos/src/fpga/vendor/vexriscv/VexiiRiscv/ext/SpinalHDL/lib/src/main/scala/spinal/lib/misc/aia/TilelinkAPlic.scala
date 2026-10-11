package spinal.lib.misc.aia

import spinal.core._
import spinal.core.fiber._
import spinal.lib._
import spinal.lib.bus.misc._
import spinal.lib.bus.tilelink
import spinal.lib.misc._
import spinal.lib.misc.slot.{Slot, SlotPool}

import scala.collection.mutable.ArrayBuffer

class MappedAPlic[T <: spinal.core.Data with IMasterSlave](
  sourceParams: Seq[APlicSourceParam],
  hartIds: Seq[Int],
  childInfos: Seq[APlicChildInfo],
  p: APlicDomainParam,
  busType: HardType[T],
  factoryGen: T => BusSlaveFactory
) extends Component {
  val io = new Bundle {
    val bus = slave(busType())
    val sources = in Bits (sourceParams.size bits)
    val mmsiaddrcfg = (if (p.isRoot) out else in) UInt (64 bits)
    val smsiaddrcfg = (if (p.isRoot) out else in) UInt (64 bits)
    val targets = p.genParam.withDirect generate (out Bits (hartIds.size bits))
    val childSources = out Vec(childInfos.map(childInfo => Bits(childInfo.sourceIds.size bits)))
    val msiMsg = p.genParam.withMSI generate master(Stream(APlicMsiPayload()))
  }

  if (p.isRoot && (p.genParam.withMSI || p.genParam._withMsiAddrcfg)) {
    io.mmsiaddrcfg.assignDontCare()
    io.smsiaddrcfg.assignDontCare()
  }

  val aplic = APlic(p, sourceParams, hartIds, childInfos)

  aplic.sources := io.sources
  if (p.genParam.withDirect) {
    io.targets := aplic.direct.targets
  }
  io.childSources := aplic.childSources

  if (p.isRoot) {
    io.mmsiaddrcfg := aplic.mmsiaddrcfg
    io.smsiaddrcfg := aplic.smsiaddrcfg
  } else {
    aplic.mmsiaddrcfg := io.mmsiaddrcfg
    aplic.smsiaddrcfg := io.smsiaddrcfg
  }

  if (p.genParam.withMSI) {
    io.msiMsg << aplic.msi.msiStream
  }

  val factory = factoryGen(io.bus)
  val mapping = APlicMapper(factory)(aplic)
}

case class TilelinkAPlic(sourceParams: Seq[APlicSourceParam], hartIds: Seq[Int],
                         childInfos: Seq[APlicChildInfo],
                         domainParam: APlicDomainParam, params: tilelink.BusParameter)
                         extends MappedAPlic[tilelink.Bus](
  sourceParams,
  hartIds,
  childInfos,
  domainParam,
  new tilelink.Bus(params),
  new tilelink.SlaveFactory(_, true)
)

case class TilelinkAPlicMsiSender(pendingSize: Int, busParams: tilelink.BusParameter) extends Component {
  val io = new Bundle {
    val msiMsg = slave(Stream(APlicMsiPayload()))
    val bus = master(tilelink.Bus(busParams))
  }

  val slots = new SlotPool(pendingSize, true)(new Slot)

  val out = io.msiMsg.map(payload => {
    val channelA = cloneOf(io.bus.a.payloadType)
    channelA.opcode   := tilelink.Opcode.A.PUT_FULL_DATA
    channelA.size     := 2
    channelA.source   := slots.allocate.id
    channelA.address  := payload.address.resized
    channelA.data     := payload.data.asBits.resized
    channelA.debugId  := 0
    channelA.mask     := 0xf
    channelA
  }).haltWhen(slots.allocate.full)

  when(out.fire) {
    slots.allocate{s => {}}
  }

  io.bus.a <-< out
  io.bus.d.ready := True

  val slotReader = slots.slots.reader(io.bus.d.source)
  when (io.bus.d.fire && io.bus.d.isLast()) {
    slots.free(io.bus.d.source)
  }
}

object TilelinkAPlic {
  def getTilelinkSlaveSupport(proposed: bus.tilelink.M2sSupport, addressWidth: Int = 20) = bus.tilelink.SlaveFactory.getSupported(
    addressWidth = addressWidth,
    dataWidth = 32,
    allowBurst = false,
    proposed
  )

  def addressWidth(maxTargetId: Int): Int = {
    import APlicMapping._
    val aplicSize = idcOffset + maxTargetId * idcGroupSize
    return log2Up(aplicSize)
  }
}

object TilelinkAPlicMsiSenderFiber {
  def getTilelinkMasterSupport(pendingSize: Int, addressWidth: Int, name: Nameable) = bus.tilelink.M2sParameters(
    addressWidth = addressWidth,
    dataWidth = 32,
    masters = List(
      tilelink.M2sAgent(
        name = name,
        mapping = List(
          tilelink.M2sSource(
            id = SizeMapping(0, pendingSize),
            emits = tilelink.M2sTransfers(
              putFull = tilelink.SizeRange(4)
            )
          )
        )
      )
    )
  )
}

case class TilelinkAPlicMsiSenderFiber(pendingSize: Int = 4, addressWidth: Int = 64) extends Area with APlicMsiConsumerFiber {
  val node = tilelink.fabric.Node.down()
  var msiStream: Option[Stream[APlicMsiPayload]] = None

  override def createMsiStreamConsumer(): Stream[APlicMsiPayload] = {
    if (msiStream.isEmpty) {
      msiStream = Some(Stream(APlicMsiPayload()))
    }

    msiStream.get
  }

  val thread = Fiber build new Area {
    val busParams = TilelinkAPlicMsiSenderFiber.getTilelinkMasterSupport(pendingSize, addressWidth, TilelinkAPlicMsiSenderFiber.this)

    node.m2s forceParameters busParams
    node.s2m.supported load tilelink.S2mSupport.none()

    val core = TilelinkAPlicMsiSender(pendingSize, node.bus.p)

    core.io.bus <> node.bus
    core.io.msiMsg << msiStream.get
  }
}

case class TilelinkAPlicFiber(domainParam: APlicDomainParam) extends Area with CascadedInterruptCtrlFiber with APlicMsiProducerFiber {
  val node = tilelink.fabric.Node.up()
  val core = Handle[TilelinkAPlic]()

  case class SourceSpec(node: InterruptNode, param: APlicSourceParam)
  case class TargetSpec(node: InterruptNode, id: Int)

  val sources = ArrayBuffer[SourceSpec]()
  val targets = ArrayBuffer[TargetSpec]()
  var msiStream: Option[Stream[APlicMsiPayload]] = None
  val mmsiaddrcfg = UInt (64 bits)
  val smsiaddrcfg = UInt (64 bits)

  override def defaultInterruptMode = LEVEL_HIGH

  override def createMsiStreamProducer(): Stream[APlicMsiPayload] = {
    if (msiStream.isEmpty) {
      msiStream = Some(Stream(APlicMsiPayload()))
    }

    msiStream.get
  }

  override def createInterruptMaster(id: Int) : InterruptNode = {
    val spec = node.clockDomain on TargetSpec(InterruptNode.master(), id)
    targets += spec
    spec.node
  }

  override def createInternalInterruptSlave(id: Int, mode: InterruptMode) : InterruptNode = {
    val param = APlicSourceParam(id, mode)
    val spec = node.clockDomain on SourceSpec(InterruptNode.slave(), param)
    sources += spec
    spec.node
  }

  val thread = Fiber build new Area {
    lock.await()

    node.m2s.supported.load(TilelinkAPlic.getTilelinkSlaveSupport(node.m2s.proposed, TilelinkAPlic.addressWidth(targets.map(_.id).max + 1)))
    node.s2m.none()

    val childSources = childs.zipWithIndex.map{case (child, i) => APlicChildInfo(i, child.sourceIds)}

    val aplic = TilelinkAPlic(sources.map(_.param).toSeq, targets.map(_.id).toSeq, childSources.toSeq, domainParam, node.bus.p)

    core.load(aplic)

    if (domainParam.genParam.withMSI) {
      msiStream.get << core.io.msiMsg
    }

    core.io.bus <> node.bus
    core.io.sources := sources.map(_.node.flag).asBits()
    if (domainParam.genParam.withDirect) {
      Vec(targets.map(_.node.flag)) := core.io.targets.asBools
    }

    if (domainParam.isRoot) {
      mmsiaddrcfg := core.io.mmsiaddrcfg
      smsiaddrcfg := core.io.smsiaddrcfg
    } else {
      core.io.mmsiaddrcfg := mmsiaddrcfg
      core.io.smsiaddrcfg := smsiaddrcfg
    }

    for((child, ioSlaveSource) <- childs.zip(core.io.childSources)) {
      child.interruptNodes.zipWithIndex.map{case (node, i) =>
        node.flag := ioSlaveSource(i)
      }
    }
  }
}
