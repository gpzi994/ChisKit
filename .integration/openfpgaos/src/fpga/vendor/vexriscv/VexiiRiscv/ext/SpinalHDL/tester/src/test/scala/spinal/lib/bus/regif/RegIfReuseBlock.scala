package spinal.lib.bus.regif

import spinal.core._
import spinal.lib._
import spinal.lib.bus.amba3.apb._
import spinal.lib.bus.amba4.apb._
import spinal.lib.bus.localbus._
import spinal.lib.bus.regif.AccessType._

class RegIfReuseBlock extends Component {
  val io = new Bundle {
    val apb = slave(Apb4(Apb4Config(16, 32)))
    val ram = Vec(master(MemBus(MemBusConfig(aw = 2, dw = 32))), 8)
  }

  val busif = BusInterface(io.apb, (0x000, 4 KiB), 0, regPre = "AP")

  val REG0 = busif.newReg("reg0")
  val reg32bit_0 = REG0.field(Bits(32 bit), RW, 0, doc = "field0 test ...")

  val REG1 = busif.newReg("reg1")
  val reg32bit_1 = REG1.field(Bits(32 bit), RW, 0, doc = "field1 test ...")

  (0 to 7).foreach{i =>
    val tagAddr = 0x200 + i *4*(2 + 4)
    busif.newBlockTagAt(tagAddr, s"r${i}")("Turbo")
    new Area {
      val Reg = busif.newRegAt(tagAddr, "rega")(SymbolName(s"RegA"))
      val field0 = Reg.field(Bits(2 bits), RW, 0, doc = "inter Row number\n0:5,1:10,2:20,3:20other").asOutput()
      val field1 = Reg.field(Bits(2 bits), RW, 0, doc = "CP relation\n0: C=P-1\n1: C=p\n2: C=p+1").asOutput()
      val Reg2 = busif.newReg(doc = "Turbo CRC Poly")(SymbolName(s"RegB"))
      val crc_mode = Reg2.field(Bool(), RW, 1, doc = "0: CRC24; 1: CRC16").asOutput()
      val crc_poly = Reg2.field(Bits(24 bit), RW, 0x864cfb, doc = "(D24+D23+D18+D17+D14+D11+D10+D7+D6+D5+D4+D3+D+1)").asOutput()
      val RAM = busif.newRAM(16, doc = "Turbo CRC Poly")(SymbolName(s"RAM"))
      RAM.field(32, doc = "0: CRC24; 1: CRC16")("ram")
      RAM.bus >> io.ram(i)
    }
    busif.resetBlockTag()
  }

  (0 to 4).foreach{ i =>
    busif.newBlockTag( s"r${i}")("MYBLK")
    val MYREG = busif.newReg("myreg")(SymbolName(s"MYREG${i}"))
    val myreg = MYREG.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"myreg${i}")).asOutput()
    busif.resetBlockTag()
  }


  busif.newBlockTagAt(0x2E0, s"r${9}")("MYBLK")
  val MYREG = busif.newRegAt(0x2E0, "myreg")(SymbolName(s"MYREG${9}"))
  val myreg = MYREG.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"myreg${9}")).asOutput()
  val MYREG1 = busif.newReg("myreg")(SymbolName(s"MYREG${10}"))
  val myreg1 = MYREG1.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"myreg${10}")).asOutput()
  busif.resetBlockTag()

  (0 to 4).foreach{i =>
    busif.newBlockTag( s"r${20+i}")("MYBLK")
    val MYREG = busif.newRegAt(0x400 + i *4*2, "myreg")(SymbolName(s"MYREG${20+i}"))
    val myreg = MYREG.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"myreg${20+i*2}")).asOutput()
    val MYREG1 = busif.newReg("myreg")(SymbolName(s"MYREG${9}"))
    val myreg1 = MYREG1.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"myreg${20+i*2+1}")).asOutput()
    busif.resetBlockTag()
  }


  val REG2 = busif.newRegAt(0x500, "reg2")
  val reg32bit_2 = REG2.field(Bits(32 bit), RW, 0, doc = "field2 test ...")

  (0 to 5).foreach { i =>
    busif.newBlockTag(s"b${+ i}")("BLKTT")
    val FRAME0 = busif.newRegAt(0x600 + i * 4 * 4, "frame0")(SymbolName(s"FRAME${i*4}"))
    val frame0 = FRAME0.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"frame${i*4}")).asOutput()
    val FRAME1 = busif.newReg("frame1")(SymbolName(s"FRAME${i*4+1}"))
    val frame1 = FRAME1.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"myreg${i*4+1}")).asOutput()
    val FRAME2 = busif.newReg("frame2")(SymbolName(s"FRAME${i*4+2}"))
    val frame2 = FRAME2.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"myreg${i*4+2}")).asOutput()
    busif.resetBlockTag()
  }

  (0 to 3).foreach { i =>
    busif.newBlockTag(s"bi${+ i}")("BLKTT")
    val FRAME0 = busif.newRegAt(0x700 + i * 4 * 8, "frame0")(SymbolName(s"FRAME${i*4}"))
    val frame0 = FRAME0.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"frame${i*4}")).asOutput()
    val FRAME1 = busif.newReg("frame1")(SymbolName(s"FRAME${i*4+1}"))
    val frame1 = FRAME1.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"myreg${i*4+1}")).asOutput()
    val FRAME2 = busif.newReg("frame2")(SymbolName(s"FRAME${i*4+2}"))
    val frame2 = FRAME2.field(Bits(32 bit), RW, 0 , doc = "...")(SymbolName(s"myreg${i*4+2}")).asOutput()
    busif.resetBlockTag()
  }

  val REG3 = busif.newRegAt(0x2f0, "reg2")
  val REG4 = busif.newRegAt(0x2fc, "reg2")

  busif.accept(DocHtml("reuseblk"))
  busif.accept(DocJson("reuseblk"))
  busif.accept(DocRalf("reuseblk"))
  busif.accept(DocRalf("reuseblk2", flatten = true))
  busif.accept(DocCHeader("reuseblk", ""))
  busif.accept(DocSVHeader("reuseblk", ""))
  busif.accept(DocSystemRdl("reuseblk"))
}

object RegIfReuseBlockTesterMain extends App{
  val sp = SpinalConfig()
    .copy(targetDirectory = "./out/regifreuse")
  sp.generateVerilog(new RegIfReuseBlock)
}
