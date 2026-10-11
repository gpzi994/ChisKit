package spinal.lib.misc.aia

import spinal.core._
import spinal.lib._

case class APlicMsiParam(
  base: BigInt = 0,
  hhxs: Int = 0,
  lhxs: Int = 0,
  hhxw: Int = 0,
  lhxw: Int = 0
)

case class APlicGenParam(withDirect: Boolean,
                         withMSI: Boolean,
                         genIEP: Boolean = true,
                         withIForce: Boolean = false,
                         var _MMsiParams: APlicMsiParam = APlicMsiParam(),
                         var _SMsiParams: APlicMsiParam = APlicMsiParam(),
                         var _withMsiAddrcfg: Boolean = false,
                         var _lockMSI: Boolean = false) {

  def lockMSI(): this.type = {
    this._lockMSI = true
    this
  }

  def withMsiAddrcfg(): this.type = {
    this._withMsiAddrcfg = true
    this
  }

  def withMachineMsiParams(param: APlicMsiParam): this.type = {
    this._MMsiParams = param
    this
  }

  def withMachineMsiParams(address: BigInt = 0, hhxs: Int = 0, lhxs: Int = 0, hhxw: Int = 0, lhxw: Int = 0): this.type = {
    withMachineMsiParams(APlicMsiParam(base = address, hhxs = hhxs, lhxs = lhxs, hhxw = hhxw, lhxw = lhxw))
  }

  def withSupervisorMsiParams(param: APlicMsiParam): this.type = {
    this._SMsiParams = param
    this
  }

  def withSupervisorMsiParams(address: BigInt = 0, lhxs: Int = 0): this.type = {
    withSupervisorMsiParams(APlicMsiParam(base = address, lhxs = lhxs))
  }
}

object APlicGenParam {
  def test = APlicGenParam(
    withDirect  = true,
    withMSI     = true,
    genIEP      = true,
    withIForce  = true
  )

  def full = APlicGenParam(
    withDirect  = true,
    withMSI     = true,
    genIEP      = true
  )

  def msi = APlicGenParam(
    withDirect  = false,
    withMSI     = true
  )

  def direct = APlicGenParam(
    withDirect  = true,
    withMSI     = false
  )
}

case class APlicDomainParam(isRoot: Boolean,
                            isMDomain: Boolean,
                            genParam: APlicGenParam)

object APlicDomainParam {
  def root(genParam: APlicGenParam) = APlicDomainParam(
    isRoot    = true,
    isMDomain = true,
    genParam  = genParam
  )

  def M(genParam: APlicGenParam) = APlicDomainParam(
    isRoot    = false,
    isMDomain = true,
    genParam  = genParam
  )

  def S(genParam: APlicGenParam) = APlicDomainParam(
    isRoot    = false,
    isMDomain = false,
    genParam  = genParam
  )
}

case class APlicChildInfo(childIdx: Int, sourceIds: Seq[Int])

case class APlicMsiPayload() extends Bundle {
  val address = UInt(64 bits)
  val data = UInt(32 bits)
}

trait APlicMsiProducerFiber extends Nameable{
  def createMsiStreamProducer(): Stream[APlicMsiPayload]
}

trait APlicMsiConsumerFiber extends Nameable{
  def createMsiStreamConsumer(): Stream[APlicMsiPayload]
}

case class APlic(p: APlicDomainParam,
                 sourceParams: Seq[APlicSourceParam],
                 hartIds: Seq[Int],
                 childInfos: Seq[APlicChildInfo]) extends Area {
  val sourceIds = sourceParams.map(_.id)
  require(sourceIds.distinct.size == sourceIds.size, "APlic requires no duplicate interrupt source")
  require(hartIds.distinct.size == hartIds.size, "APlic requires no duplicate harts")
  require(p.genParam.withDirect || p.genParam.withMSI, "At least one delivery mode should be enabled")

  val sources = Bits(sourceIds.size bits)
  val childSources = Vec(childInfos.map(childInfo => Bits(childInfo.sourceIds.size bits)))
  val mmsiaddrcfg = UInt(64 bits)
  val smsiaddrcfg = UInt(64 bits)

  val childInterruptIds = childInfos.flatMap(childInfo => childInfo.sourceIds).distinct
  val interruptDelegatable = for (sourceId <- sourceIds) yield childInterruptIds.find(_ == sourceId).isDefined

  val deliveryEnable = RegInit(False)
  val isMSI = ((p.genParam.withDirect, p.genParam.withMSI): @unchecked) match {
    case (true, true) => RegInit(False)
    case (false, true) => True
    case (true, false) => False
  }
  val bigEndian = False

  val interrupts: Seq[APlicSource] = for (((param, delegatable), i) <- sourceParams.zip(interruptDelegatable).zipWithIndex)
    yield APlicSource(param, APlicSourceState(delegatable, p.genParam.withDirect, p.genParam.withMSI, isMSI, sources(i)))

  val childMappings = for ((childInfo, childSource) <- childInfos.zip(childSources)) yield new Area {
    for ((childSourceId, idx) <- childInfo.sourceIds.zipWithIndex) yield new Area {
      interrupts.find(_.id == childSourceId).map(interrupt => new Area {
        when(interrupt.delegated && (Bool(childInfos.size == 1) || interrupt.childIdx === childInfo.childIdx)) {
          childSource(idx) := interrupt.input
        } otherwise {
          childSource(idx) := False
        }
      })
    }
  }

  val msiaddrcfg = (p.genParam.withMSI || p.genParam._withMsiAddrcfg) generate new Area {
    val m = new Area {
      val (lock, hhxs, lhxs, hhxw, lhxw, ppn) = if (p.isRoot) {
        (RegInit(Bool(p.genParam._lockMSI)),
         RegInit(U(p.genParam._MMsiParams.hhxs, 5 bits)),
         RegInit(U(p.genParam._MMsiParams.lhxs, 3 bits)),
         RegInit(U(p.genParam._MMsiParams.hhxw, 3 bits)),
         RegInit(U(p.genParam._MMsiParams.lhxw, 4 bits)),
         RegInit(U(p.genParam._MMsiParams.base >> 12, 44 bits)))
      } else {
        (mmsiaddrcfg(63),
         mmsiaddrcfg(60 downto 56),
         mmsiaddrcfg(54 downto 52),
         mmsiaddrcfg(50 downto 48),
         mmsiaddrcfg(47 downto 44),
         mmsiaddrcfg(43 downto 0))
      }

      val msiaddrcfg = if (p.isRoot) {
        U(64 bits, 63            -> lock,
                  (60 downto 56) -> hhxs,
                  (54 downto 52) -> lhxs,
                  (50 downto 48) -> hhxw,
                  (47 downto 44) -> lhxw,
                  (43 downto 0)  -> ppn,
                  default        -> False)
      } else {
        mmsiaddrcfg
      }

      val maskH = (U(1) << hhxw) - 1
      val maskL = (U(1) << lhxw) - 1

      val msiaddrcfgCovered = lock.mux(
        True  -> U(64 bits, 63 -> True, default -> False),
        False -> msiaddrcfg
      )

      if (p.isRoot) {
        mmsiaddrcfg := msiaddrcfg
      }
    }

    val s = new Area {
      val (ppn, lhxs) = if (p.isRoot) {
        (RegInit(U(p.genParam._SMsiParams.base >> 12, 44 bits)),
         RegInit(U(p.genParam._SMsiParams.lhxs, 3 bits)))
      } else {
        (smsiaddrcfg(43 downto 0), smsiaddrcfg(54 downto 52))
      }

      val msiaddrcfg = if (p.isRoot) {
        U(64 bits, 63             -> m.lock,
                   (60 downto 56) -> m.hhxs,
                   (54 downto 52) -> lhxs,
                   (50 downto 48) -> m.hhxw,
                   (47 downto 44) -> m.lhxw,
                   (43 downto 0)  -> ppn,
                   default        -> False)
      } else {
        smsiaddrcfg
      }

      val msiaddrcfgCovered = m.lock.mux(
        True  -> U(0),
        False -> msiaddrcfg
      )

      if (p.isRoot) {
        smsiaddrcfg := msiaddrcfg
      }
    }

    def msiAddress(hartIndex: UInt, guestIndex: UInt = 0): UInt = {
      val groupId = (hartIndex >> m.lhxw) & m.maskH.resized
      val hartId = hartIndex & m.maskL.resized
      val groupOffset = groupId << (m.hhxs + 12)
      val lhxs = if (p.isMDomain) m.lhxs else s.lhxs
      val ppn = if (p.isMDomain) m.ppn else s.ppn
      val hartOffset = hartId << lhxs

      val msiaddr = (ppn | groupOffset.resized | hartOffset.resized | guestIndex.resized) << 12
      msiaddr
    }
  }

  val msi = p.genParam.withMSI generate new Area {
    val gateway = new APlicMSIGateway(interrupts, deliveryEnable)

    val gatewayStream = gateway.requestStream.map(req => {
      val payload = APlicMsiPayload()
      payload.address := msiaddrcfg.msiAddress(req.target.hartId, req.target.guestId).resized
      payload.data := req.target.eiid.resized
      payload
    })

    val genmsiStream = Stream(APlicMsiPayload())

    val msiStream = StreamArbiterFactory().lowerFirst.noLock.onArgs(gatewayStream, genmsiStream)
  }

  val direct = p.genParam.withDirect generate new Area {
    val gateways = for (hartId <- hartIds) yield new APlicDirectGateway(interrupts, hartId, p.genParam.withIForce)

    val targets = Mux(isMSI, B(0), gateways.map(_.iep && deliveryEnable).asBits())
  }
}

object APlic {
  def doWhenMatch(interrupts: Seq[APlicSource], id: UInt, func: APlicSource => Unit) = new Area {
    switch(id) {
      for (interrupt <- interrupts) {
        is (interrupt.id) {
          func(interrupt)
        }
      }
    }
  }

  def doClaim(interrupts: Seq[APlicSource], id: UInt) = doWhenMatch(interrupts, id, _.doClaim())

  def doSet(interrupts: Seq[APlicSource], id: UInt) = doWhenMatch(interrupts, id, _.doSet())

  def doEnable(interrupts: Seq[APlicSource], id: UInt) = doWhenMatch(interrupts, id, _.doEnable())

  def doDisable(interrupts: Seq[APlicSource], id: UInt) = doWhenMatch(interrupts, id, _.doDisable())
}
