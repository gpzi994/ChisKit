package spinal.lib.misc.aia

import spinal.core._
import spinal.lib._
import spinal.lib.bus.misc.{BusSlaveFactory, AllMapping, SingleMapping}

import scala.collection.mutable.ArrayBuffer

object APlicMapping {
  val domaincfgOffset     = 0x0000
  val sourcecfgOffset     = 0x0004
  val mmsiaddrcfgOffset   = 0x1BC0
  val mmsiaddrcfghOffset  = 0x1BC4
  val smsiaddrcfgOffset   = 0x1BC8
  val smsiaddrcfghOffset  = 0x1BCC
  val setipOffset         = 0x1C00
  val setipnumOffset      = 0x1CDC
  val in_clripOffset      = 0x1D00
  val clripnumOffset      = 0x1DDC
  val setieOffset         = 0x1E00
  val setienumOffset      = 0x1EDC
  val clrieOffset         = 0x1F00
  val clrienumOffset      = 0x1FDC
  val setipnum_leOffset   = 0x2000
  val setipnum_beOffset   = 0x2004
  val genmsiOffset        = 0x3000
  val targetOffset        = 0x3004

  val idcOffset           = 0x4000
  val idcGroupSize        = 0x20
  val ideliveryOffset     = 0x00
  val iforceOffset        = 0x04
  val ithresholdOffset    = 0x08
  val topiOffset          = 0x18
  val claimiOffset        = 0x1c
}

case class APlicGenMsiPayload() extends Bundle {
  val hartId = UInt(14 bits)
  val eiid = UInt(11 bits)
}

object APlicMapper {
  def apply(bus: BusSlaveFactory)(aplic: APlic) = new Area{
    import APlicMapping._

    val p = aplic.p

    val domaincfg = new Area {
      bus.read(U(0x80, 8 bits), address = domaincfgOffset, bitOffset = 24)
      bus.readAndWrite(aplic.deliveryEnable, address = domaincfgOffset, bitOffset = 8)
      bus.read(aplic.isMSI, address = domaincfgOffset, bitOffset = 2)
      bus.readAndWrite(aplic.bigEndian, address = domaincfgOffset, bitOffset = 0)

      if (p.genParam.withMSI && p.genParam.withDirect) {
        bus.write(aplic.isMSI, address = domaincfgOffset, bitOffset = 2)
      }
    }

    // mapping MSIADDRCFG, MSIADDRCFGH
    val msiaddrcfg = (p.genParam.withMSI || p.genParam._withMsiAddrcfg) generate new Area {
      val addrcfg = aplic.msiaddrcfg

      bus.read(addrcfg.m.msiaddrcfgCovered(31 downto 0), address = mmsiaddrcfgOffset)
      bus.read(addrcfg.m.msiaddrcfgCovered(63 downto 32), address = mmsiaddrcfghOffset)
      bus.read(addrcfg.s.msiaddrcfgCovered(31 downto 0), address = smsiaddrcfgOffset)
      bus.read(addrcfg.s.msiaddrcfgCovered(63 downto 32), address = smsiaddrcfghOffset)

      val addrcfgWrite = aplic.p.isRoot generate new Area {
        val allowWrite = !addrcfg.m.lock

        val mmsiaddrcfgh = bus.createAndDriveFlow(UInt(32 bits), mmsiaddrcfghOffset)
        when(mmsiaddrcfgh.valid && allowWrite) {
          addrcfg.m.lock := mmsiaddrcfgh.payload(31)
          addrcfg.m.hhxs := mmsiaddrcfgh.payload(28 downto 24)
          addrcfg.m.lhxs := mmsiaddrcfgh.payload(22 downto 20)
          addrcfg.m.hhxw := mmsiaddrcfgh.payload(18 downto 16)
          addrcfg.m.lhxw := mmsiaddrcfgh.payload(15 downto 12)
          addrcfg.m.ppn(43 downto 32) := mmsiaddrcfgh.payload(11 downto 0)
        }

        val mmsiaddrcfg = bus.createAndDriveFlow(UInt(32 bits), mmsiaddrcfgOffset)
        when(mmsiaddrcfg.valid && allowWrite) {
          addrcfg.m.ppn(31 downto 0) := mmsiaddrcfg.payload
        }

        val smsiaddrcfgh = bus.createAndDriveFlow(UInt(32 bits), smsiaddrcfghOffset)
        when(smsiaddrcfgh.valid && allowWrite) {
          addrcfg.s.lhxs := smsiaddrcfgh.payload(22 downto 20)
          addrcfg.s.ppn(43 downto 32) := smsiaddrcfgh.payload(11 downto 0)
        }

        val smsiaddrcfg = bus.createAndDriveFlow(UInt(32 bits), smsiaddrcfgOffset)
        when(smsiaddrcfg.valid && allowWrite) {
          addrcfg.s.ppn(31 downto 0) := smsiaddrcfg.payload
        }
      }
    }

    // mapping GENMSI
    val msi = p.genParam.withMSI generate new Area {
      val logic = aplic.msi
      val addrcfg = aplic.msiaddrcfg

      val genmsiPayload = Flow(APlicGenMsiPayload())
      val genmsiFlow = bus.createAndDriveFlow(UInt(32 bits), genmsiOffset).discardWhen(genmsiPayload.valid || !aplic.isMSI)

      // use register to store and wrap genmsiFlow's value
      val rGenmsiFlow = new Area {
        val valid = RegInit(False)
        val hartId = RegInit(U(0, 14 bits))
        val eiid = RegInit(U(0, 11 bits))

        when(genmsiFlow.valid) {
          valid := True
          hartId := genmsiFlow.payload(31 downto 18)
          eiid := genmsiFlow.payload(10 downto 0)
        }
      }

      genmsiPayload.valid := rGenmsiFlow.valid
      genmsiPayload.payload.hartId := rGenmsiFlow.hartId
      genmsiPayload.payload.eiid := rGenmsiFlow.eiid

      val genmsiPayloadStream = Stream(APlicGenMsiPayload())
      genmsiPayloadStream.valid := genmsiPayload.valid && !logic.gateway.requestStreamValidMask
      genmsiPayloadStream.payload := genmsiPayload.payload

      when(genmsiPayloadStream.fire) {
        rGenmsiFlow.valid := False
        rGenmsiFlow.hartId := 0
        rGenmsiFlow.eiid := 0
      }

      val genmsiStream = genmsiPayloadStream.map(param => {
        val payload = APlicMsiPayload()
        payload.address := addrcfg.msiAddress(param.hartId).resized
        payload.data := param.eiid.resized
        payload
      })

      logic.genmsiStream << genmsiStream

      bus.read(genmsiPayload.payload.hartId, address = genmsiOffset, bitOffset = 18)
      bus.read(genmsiPayload.valid, address = genmsiOffset, bitOffset = 12)
      bus.read(genmsiPayload.payload.eiid, address = genmsiOffset, bitOffset = 0)
    }

    // mapping SETIPNUM, CLRIPNUM, SETIENUM, CLRIPNUM
    val numOPs = new Area {
      def mapNumArea(offset: Int, func: UInt => Unit, doc: String = null) = new Area {
        val numFlow = bus.createAndDriveFlow(UInt(32 bits), address = offset, documentation = doc)
        when(numFlow.valid) {
          func(numFlow.payload)
        }
      }

      val setipnum = mapNumArea(setipnumOffset, APlic.doSet(aplic.interrupts, _))
      val setipnum_le = mapNumArea(setipnum_leOffset, APlic.doSet(aplic.interrupts, _))
      val clripnum = mapNumArea(clripnumOffset, APlic.doClaim(aplic.interrupts, _))
      val setienum = mapNumArea(setienumOffset, APlic.doEnable(aplic.interrupts, _))
      val clrienum = mapNumArea(clrienumOffset, APlic.doDisable(aplic.interrupts, _))
    }

    bus.read(B(0), address = setipOffset, bitOffset = 0)
    bus.read(B(0), address = setieOffset, bitOffset = 0)
    bus.read(B(0), address = clrieOffset, bitOffset = 0)
    bus.read(B(0), address = in_clripOffset, bitOffset = 0)

    // mapping SOURCECFG, TARGET, SETIE, SETIP for interrupt
    val interruptMapping = for(interrupt <- aplic.interrupts) yield new Area {
      val interruptOffset = (interrupt.id / bus.busDataWidth) * bus.busDataWidth / 8
      val interruptBitOffset = interrupt.id % bus.busDataWidth
      val configOffset = (interrupt.id - 1) * 4

      val source = new Area {
        val configFlow = bus.createAndDriveFlow(UInt(11 bits), sourcecfgOffset + configOffset)
        when(configFlow.valid) {
          interrupt.setConfig(configFlow.payload)
        }
      }

      val target = new Area {
        val configFlow = bus.createAndDriveFlow(UInt(18 bits), address = targetOffset + configOffset, bitOffset = 0)

        when(configFlow.valid && interrupt.isActive) {
          if(p.genParam.withDirect) {
            when(!aplic.isMSI) {
              val prio = configFlow.payload(7 downto 0)
              interrupt.direct.prio := (prio === 0) ? U(1) | prio
            }
          }
          if(p.genParam.withMSI) {
            when(aplic.isMSI) {
              interrupt.msi.guestId := configFlow.payload(17 downto 12)
              interrupt.msi.eiid := configFlow.payload(10 downto 0)
            }
          }
        }

        val configViewList = ArrayBuffer[(Bool, Bits)]()
        if (p.genParam.withDirect) {
          val view = B(18 bits, (7 downto 0) -> interrupt.direct.prio.asBits,
                                default -> False)
          configViewList.addRet((False, view))
        }
        if (p.genParam.withMSI) {
          val view = B(18 bits, (17 downto 12) -> interrupt.msi.guestId.asBits,
                                (10 downto 0) -> interrupt.msi.eiid.asBits,
                                default -> False)
          configViewList.addRet((True, view))
        }
        val configView = aplic.isMSI.muxListDc(configViewList)

        bus.read(configView, address = targetOffset + configOffset, bitOffset = 0)

        val targetFlow = bus.createAndDriveFlow(interrupt.targetId, address = targetOffset + configOffset, bitOffset = 18)

        when(targetFlow.valid && interrupt.isActive) {
          interrupt.targetId := targetFlow.payload
        }

        bus.read(interrupt.targetId, address = targetOffset + configOffset, bitOffset = 18)
      }

      val iep = p.genParam.genIEP generate new Area {
        bus.readAndWrite(interrupt.ie, address = setieOffset + interruptOffset, bitOffset = interruptBitOffset)

        bus.read(interrupt.ip, address = setipOffset + interruptOffset, bitOffset = interruptBitOffset)
        val ipDrive = bus.createAndDriveFlow(Bool(), address = setipOffset + interruptOffset, bitOffset = interruptBitOffset)
        when(ipDrive.valid) {
          interrupt.doPendingUpdate(ipDrive.payload)
        }

        bus.read(interrupt.rectified, address = in_clripOffset + interruptOffset, bitOffset = interruptBitOffset)
        val clripDrive = bus.createAndDriveFlow(Bool(), address = in_clripOffset + interruptOffset, bitOffset = interruptBitOffset)
        when(clripDrive.valid && clripDrive.payload === True) {
          interrupt.doClaim()
        }

        val clrieDrive = bus.createAndDriveFlow(Bool(), address = clrieOffset + interruptOffset, bitOffset = interruptBitOffset)
        when(clrieDrive.valid && clrieDrive.payload === True) {
          interrupt.doDisable()
        }
      }
    }

    // mapping interrupt delivery control for each gateway
    val direct = p.genParam.withDirect generate new Area {
      val idcs =  for(idc <- aplic.direct.gateways) yield new Area {
        val idcThisOffset = idcOffset + (idc.hartId * idcGroupSize)
        val nowRequest = idc.bestRequest.asInstanceOf[APlicDirectRequest]

        bus.readAndWrite(idc.idelivery, address = idcThisOffset + ideliveryOffset)
        if (p.genParam.withIForce) {
          bus.readAndWrite(idc.iforce, address = idcThisOffset + iforceOffset)
        }
        bus.readAndWrite(idc.ithreshold, address = idcThisOffset + ithresholdOffset)

        // topi readonly
        bus.read(nowRequest.prio, address = idcThisOffset + topiOffset, bitOffset = 0)
        bus.read(nowRequest.id, address = idcThisOffset + topiOffset, bitOffset = 16)

        // reading claimi trigger clean the top interrupt
        bus.read(nowRequest.prio, address = idcThisOffset + claimiOffset, bitOffset = 0)
        bus.read(nowRequest.id, address = idcThisOffset + claimiOffset, bitOffset = 16)
        bus.onRead(address = idcThisOffset + claimiOffset) {
          idc.doBestClaim()
        }
      }
    }
  }
}
