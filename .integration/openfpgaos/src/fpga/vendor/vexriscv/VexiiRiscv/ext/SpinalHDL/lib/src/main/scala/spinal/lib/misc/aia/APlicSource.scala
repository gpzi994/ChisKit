package spinal.lib.misc.aia

import spinal.core._
import spinal.lib._
import spinal.lib.misc._

import scala.collection.mutable.LinkedHashMap

abstract class APlicGenericRequest(idWidth: Int) extends Bundle {
  val id = UInt(idWidth bits)
  val valid = Bool()

  def prioritize(other: APlicGenericRequest): Bool
  def pending(threshold: UInt): Bool
  def dummy(): APlicGenericRequest
  def verify(cond: Bool): APlicGenericRequest = {
    Mux(cond, this, dummy())
  }
}

/**
 * Trigger mode for interrupt source
 */
object APlicSourceMode extends SpinalEnum {
  val INACTIVE, DETACHED, EDGE1, EDGE0, LEVEL1, LEVEL0 = newElement()
  val RESERVED2, RESERVED3 = newElement()
  defaultEncoding = SpinalEnumEncoding("sm")(
    INACTIVE -> 0,
    DETACHED -> 1,
    RESERVED2 -> 2,
    RESERVED3 -> 3,
    EDGE1 -> 4,
    EDGE0 -> 5,
    LEVEL1 -> 6,
    LEVEL0 -> 7)
}

case class APlicDirectRequest(idWidth: Int, priorityWidth: Int) extends APlicGenericRequest(idWidth) {
  val prio = UInt(priorityWidth bits)

  override def prioritize(other: APlicGenericRequest): Bool = {
    val x = other.asInstanceOf[APlicDirectRequest]
    val prioCheck = (prio < x.prio) || ((prio === x.prio) && (id <= x.id))

    // !x.valid || (valid && prioCheck)
    !x.valid || (valid && ((x.id === 0) || ((id =/= 0) && prioCheck)))
  }

  override def pending(threshold: UInt): Bool = {
    val prioCheck = (threshold === 0) || (prio < threshold)

    // vaild && prioCheck
    valid && ((id === 0) || ((id =/= 0) && prioCheck ))
  }

  override def dummy(): APlicGenericRequest = {
    val tmp = APlicDirectRequest(idWidth, priorityWidth)
    tmp.id := 0
    tmp.valid := False
    tmp.prio := 0
    tmp
  }
}

case class APlicMSITarget() extends Bundle {
  val hartId = UInt(14 bits)
  val guestId = UInt(6 bits)
  val eiid = UInt(11 bits)
}

case class APlicMSIRequest(idWidth: Int) extends APlicGenericRequest(idWidth) {
  val target = APlicMSITarget()
  val keep = RegNext(valid) init(False)

  override def prioritize(other: APlicGenericRequest): Bool = {
    val x = other.asInstanceOf[APlicMSIRequest]
    !x.valid || (valid && id <= x.id)
  }

  override def pending(threshold: UInt): Bool = {
    valid
  }

  override def dummy(): APlicGenericRequest = {
    val tmp = APlicMSIRequest(idWidth)
    tmp.target.assignDontCare()
    tmp.id := 0
    tmp.valid := False
    tmp
  }
}

case class APlicSourceParam(id: Int, mode: InterruptMode)

case class APlicSourceState(
  withDelegation: Boolean,
  withDirect: Boolean,
  withMSI: Boolean,
  msiState: Bool,
  input: Bool
)

abstract class APlicSource(sourceId: Int, state: APlicSourceState) extends Area {
  import APlicSourceMode._

  val id = sourceId
  val input = state.input
  val ie = RegInit(False)
  val ip = RegInit(False)
  val isMSI = state.msiState
  val config = RegInit(U(0, 11 bits))
  val delegated = config(10)
  val childIdx = config(9 downto 0)
  val modeBit = delegated ? APlicSourceMode.INACTIVE.asBits | config(2 downto 0).asBits
  val mode = APlicSourceMode()

  mode.assignFromBits(modeBit)

  /* target field */
  val targetId = RegInit(U(0x0, 14 bits))
  val direct = state.withDirect generate new Area {
    val prio = RegInit(U(1, 8 bits))
  }

  val msi = state.withMSI generate new Area {
    val guestId = RegInit(U(0x0, 6 bits))
    val eiid = RegInit(U(0x0, 11 bits))
  }

  val rectified = Bool()
  val rectifiedCtx = WhenBuilder()
  driveRectifiedCtx(rectifiedCtx)
  rectifiedCtx.otherwise{
    rectified := False
  }
  val allowSet = Bool()
  val allowClear = Bool()
  val allowCtx = WhenBuilder()
  driveAllowCtx(allowCtx)
  allowCtx.otherwise{
    allowSet := False
    allowClear := False
  }

  val iepCtx = WhenBuilder()
  driveIepCtx(iepCtx)
  iepCtx.otherwise {
    ip := False
    ie := False
  }

  def driveAllowCtx(ctx: WhenBuilder): Unit
  def driveIepCtx(ctx: WhenBuilder): Unit
  def driveRectifiedCtx(ctx: WhenBuilder): Unit
  def driveConfigUpdateCtx(ctx: WhenBuilder, nextMode: APlicSourceMode.C): Unit
  def supportModes(): Seq[APlicSourceMode.E]

  def asDirectRequest(idWidth: Int, targetHart: Int): APlicGenericRequest = {
    val ret = new APlicDirectRequest(idWidth, direct.prio.getWidth)
    val enable = ie && !isMSI
    ret.id := U(id)
    ret.valid := ip && enable && (targetId === targetHart)
    ret.prio := direct.prio
    ret
  }

  def asMSIRequest(idWidth: Int): APlicGenericRequest = {
    val ret = new APlicMSIRequest(idWidth)
    val enable = (ie || ret.keep) && isMSI
    ret.target.hartId := targetId
    ret.target.guestId := msi.guestId
    ret.target.eiid := msi.eiid
    ret.id := U(id)
    ret.valid := ip && enable
    ret
  }

  def doClaim(): Unit = {
    ip.clearWhen(allowClear)
  }

  def doSet(): Unit = {
    ip.setWhen(allowSet)
  }

  def doPendingUpdate(pending: Bool): Unit = {
    when(pending) {
      doSet()
    } otherwise {
      doClaim()
    }
  }

  def doEnable(): Unit = {
    ie := True
  }

  def doDisable(): Unit = {
    ie := False
  }

  def doEnableUpdate(enabled: Bool): Unit = {
    when(enabled) {
      doEnable()
    } otherwise {
      doDisable()
    }
  }

  def isActive: Bool = mode =/= INACTIVE

  def doConfigIpUpdate(modeB: Bits) {
    val mode = APlicSourceMode()
    val ctx = WhenBuilder()

    mode.assignFromBits(modeB)

    ctx.when(mode === INACTIVE) {
      ip := False
    }

    driveConfigUpdateCtx(ctx, mode)
  }

  def setConfig(payload: UInt): Unit = {
    val _delegated = payload(10)

    when (_delegated) {
      if (state.withDelegation) {
        config := payload
      } else {
        config := 0
      }
    } otherwise {
      val _mode = payload(2 downto 0)

      switch (_mode) {
        for (state <- supportModes()) {
          is(state.asBits.asUInt) {
            config := payload(2 downto 0).resized
            doConfigIpUpdate(state.asBits)
          }
        }

        default {
          config := 0
        }
      }
    }
  }
}

object APlicSource {
  def apply(param: APlicSourceParam, state: APlicSourceState): APlicSource = {
    val sourceId = param.id

    param.mode match {
      case EDGE_FALLING => new APlicSourceActiveFalling(sourceId, state)
      case EDGE_RISING  => new APlicSourceActiveRising(sourceId, state)
      case LEVEL_HIGH   => new APlicSourceActiveHigh(sourceId, state)
      case LEVEL_LOW    => new APlicSourceActiveLow(sourceId, state)
      case SPURIOUS     => new APlicSourceActiveSpurious(sourceId, state)
    }
  }
}

case class APlicFullSource(sourceId: Int, state: APlicSourceState) extends APlicSource(sourceId, state) {
  import APlicSourceMode._

  lazy val trigger = input.edges()

  override def supportModes(): Seq[E] = Seq(INACTIVE, EDGE0, EDGE1, LEVEL0, LEVEL1, DETACHED)

  override def driveRectifiedCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === EDGE1) {
      rectified := trigger.rise
    }
    ctx.when(mode === EDGE0) {
      rectified := trigger.fall
    }
    ctx.when(mode === LEVEL1) {
      rectified := input
    }
    ctx.when(mode === LEVEL0) {
      rectified := ~input
    }
  }

  override def driveAllowCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === DETACHED) {
      allowSet := True
      allowClear := True
    }
    ctx.when(mode === EDGE1) {
      allowSet := True
      allowClear := True
    }
    ctx.when(mode === EDGE0) {
      allowSet := True
      allowClear := True
    }
    ctx.when(mode === LEVEL1) {
      allowSet := Mux(isMSI, rectified, False)
      allowClear := Mux(isMSI, True, False)
    }
    ctx.when(mode === LEVEL0) {
      allowSet := Mux(isMSI, rectified, False)
      allowClear := Mux(isMSI, True, False)
    }
  }

  override def driveIepCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === DETACHED) {
    }
    ctx.when(mode === EDGE1) {
      ip.setWhen(trigger.rise)
    }
    ctx.when(mode === EDGE0) {
      ip.setWhen(trigger.fall)
    }
    ctx.when(mode === LEVEL1 && !isMSI) {
      ip := input
    }
    ctx.when(mode === LEVEL1 && isMSI) {
      ip.setWhen(trigger.rise)
      ip.clearWhen(!input)
    }
    ctx.when(mode === LEVEL0 && !isMSI) {
      ip := ~input
    }
    ctx.when(mode === LEVEL0 && isMSI) {
      ip.setWhen(trigger.fall)
      ip.clearWhen(input)
    }
  }

  override def driveConfigUpdateCtx(ctx: WhenBuilder, nextMode: APlicSourceMode.C): Unit = {
    ctx.when(nextMode === EDGE1) {
      ip := trigger.rise
    }
    ctx.when(nextMode === EDGE0) {
      ip := trigger.fall
    }
    ctx.when(nextMode === LEVEL1) {
      ip := input
    }
    ctx.when(nextMode === LEVEL0) {
      ip := ~input
    }
  }
}

case class APlicSourceActiveHigh(sourceId: Int, state: APlicSourceState) extends APlicSource(sourceId, state) {
  import APlicSourceMode._

  override def supportModes(): Seq[E] = Seq(INACTIVE, LEVEL1)

  override def driveRectifiedCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === LEVEL1) {
      rectified := input
    }
  }

  override def driveAllowCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === LEVEL1) {
      allowSet := Mux(isMSI, rectified, False)
      allowClear := Mux(isMSI, True, False)
    }
  }

  override def driveIepCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === LEVEL1 && !isMSI) {
      ip := input
    }
    ctx.when(mode === LEVEL1 && isMSI) {
      ip.setWhen(input.rise())
      ip.clearWhen(!input)
    }
  }

  override def driveConfigUpdateCtx(ctx: WhenBuilder, nextMode: APlicSourceMode.C): Unit = {
    ctx.when(nextMode === LEVEL1) {
      ip := input
    }
  }
}

case class APlicSourceActiveLow(sourceId: Int, state: APlicSourceState) extends APlicSource(sourceId, state) {
  import APlicSourceMode._

  override def supportModes(): Seq[E] = Seq(INACTIVE, LEVEL0)

  override def driveRectifiedCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === LEVEL0) {
      rectified := ~input
    }
  }

  override def driveAllowCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === LEVEL0) {
      allowSet := Mux(isMSI, rectified, False)
      allowClear := Mux(isMSI, True, False)
    }
  }

  override def driveIepCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === LEVEL0 && !isMSI) {
      ip := ~input
    }
    ctx.when(mode === LEVEL0 && isMSI) {
      ip.setWhen(input.fall())
      ip.clearWhen(input)
    }
  }

  override def driveConfigUpdateCtx(ctx: WhenBuilder, nextMode: APlicSourceMode.C): Unit = {
    ctx.when(nextMode === LEVEL0) {
      ip := ~input
    }
  }
}

case class APlicSourceActiveRising(sourceId: Int, state: APlicSourceState) extends APlicSource(sourceId, state) {
  import APlicSourceMode._

  lazy val trigger = input.rise()

  override def supportModes(): Seq[E] = Seq(INACTIVE, EDGE1)

  override def driveRectifiedCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === EDGE1) {
      rectified := trigger
    }
  }

  override def driveAllowCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === EDGE1) {
      allowSet := True
      allowClear := True
    }
  }

  override def driveIepCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === EDGE1) {
      ip.setWhen(trigger)
    }
  }

  override def driveConfigUpdateCtx(ctx: WhenBuilder, mode: APlicSourceMode.C): Unit = {
    ctx.when(mode === EDGE1) {
      ip := trigger
    }
  }
}

case class APlicSourceActiveFalling(sourceId: Int, state: APlicSourceState) extends APlicSource(sourceId, state) {
  import APlicSourceMode._

  lazy val trigger = input.fall()

  override def supportModes(): Seq[E] = Seq(INACTIVE, EDGE0)

  override def driveRectifiedCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === EDGE0) {
      rectified := trigger
    }
  }

  override def driveAllowCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === EDGE0) {
      allowSet := True
      allowClear := True
    }
  }

  override def driveIepCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === EDGE0) {
      ip.setWhen(trigger)
    }
  }

  override def driveConfigUpdateCtx(ctx: WhenBuilder, nextMode: APlicSourceMode.C): Unit = {
    ctx.when(nextMode === EDGE0) {
      ip := trigger
    }
  }
}

case class APlicSourceActiveSpurious(sourceId: Int, state: APlicSourceState) extends APlicSource(sourceId, state) {
  import APlicSourceMode._

  override def supportModes(): Seq[E] = Seq(INACTIVE, DETACHED)

  override def driveRectifiedCtx(ctx: WhenBuilder): Unit = {}

  override def driveAllowCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === DETACHED) {
      allowSet := True
      allowClear := True
    }
  }

  override def driveIepCtx(ctx: WhenBuilder): Unit = {
    ctx.when(mode === DETACHED) {
    }
  }

  override def driveConfigUpdateCtx(ctx: WhenBuilder, nextMode: APlicSourceMode.C): Unit = {}
}
