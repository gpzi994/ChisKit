package spinal.lib.misc.aia

import spinal.core._
import spinal.core.fiber.{Fiber, Lock}
import spinal.lib._
import spinal.lib.bus.misc._
import spinal.lib.bus.tilelink

import scala.collection.mutable.ArrayBuffer

class MappedImsicTrigger[T <: spinal.core.Data with IMasterSlave](infos: Seq[ImsicFileInfo],
                                                           mapping: ImsicMapping,
                                                           busType: HardType[T],
                                                           factoryGen: T => BusSlaveFactory) extends Component {
  val io = new Bundle {
    val bus = slave(busType())
    val triggers = out Vec(infos.map(info => Bits(info.sourceIds.size bits)))
  }

  val factory = factoryGen(io.bus)

  val logic = ImsicTrigger(factory, mapping)(infos)

  io.triggers := logic.triggers
}

case class TilelinkImsicTrigger(infos: Seq[ImsicFileInfo],
                                mapping: ImsicMapping,
                                p: bus.tilelink.BusParameter) extends MappedImsicTrigger[bus.tilelink.Bus](
  infos,
  mapping,
  new bus.tilelink.Bus(p),
  new bus.tilelink.SlaveFactory(_, true)
)

object TilelinkImsicTrigger {
  def getTilelinkSupport(transfers: tilelink.M2sTransfers, addressWidth: Int = 20) = bus.tilelink.SlaveFactory.getSupported(
    addressWidth = addressWidth,
    dataWidth = 32,
    allowBurst = false,
    proposed = tilelink.M2sSupport(
      addressWidth = addressWidth,
      dataWidth = 32,
      transfers = transfers
    )
  )
  def getTilelinkSupport(transfers: tilelink.M2sTransfers, mapping: ImsicMapping, infos: Seq[ImsicFileInfo]): tilelink.M2sSupport = getTilelinkSupport(transfers, addressWidth(mapping, infos))

  def addressWidth(mapping: ImsicMapping, infos: Seq[ImsicFileInfo]): Int = {
    val maxGuestId = infos.map(_.guestId).max
    val maxGroupHartId = infos.map(_.groupHartId).max
    val maxGroupId = infos.map(_.groupId).max

    ImsicTrigger.addressWidth(mapping, maxGuestId, maxGroupHartId, maxGroupId)
  }
}

case class TilelinkImsicTriggerFiber(mapping: ImsicMapping = ImsicMapping()) extends Area {
  val node = bus.tilelink.fabric.Node.slave()
  val lock = Lock()

  case class ImsicFileSource(info: ImsicFileInfo) {
    val trigger = Bits(info.sourceIds.size bits)
  }

  var infos = ArrayBuffer[ImsicFileSource]()
  def addImsicFileinfo(info: ImsicFileInfo) = {
    val source = infos.addRet(ImsicFileSource(info))
    source.trigger
  }

  val thread = Fiber build new Area {
    lock.await()

    val imsicInfos = infos.map(_.info).toSeq

    node.m2s.supported.load(TilelinkImsicTrigger.getTilelinkSupport(node.m2s.proposed.transfers, mapping, imsicInfos))
    node.s2m.none()

    val core = TilelinkImsicTrigger(imsicInfos, mapping, node.bus.p)

    core.io.bus <> node.bus

    for ((info, trigger) <- infos.zip(core.io.triggers)) {
      info.trigger := trigger
    }
  }
}
