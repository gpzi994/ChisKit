package spinal.core

import spinal.core.sim._
import spinal.tester.SpinalAnyFunSuite

/**
 * Test suite for SpinalHDL simInit functionality
 * 
 * Tests simInit method which provides simulation-only register initialization
 * covering basic functionality, data type compatibility, chaining with init(),
 * composite types, reset type compatibility, and error conditions.
 */
class SimInitTest_copy extends SpinalAnyFunSuite {

  test("basic simInit functionality") {
    class BasicSimInitComponent extends Component {
      val counter8  = Reg(UInt(8 bits)).simInit(0x42)
      val counter16 = Reg(UInt(16 bits)).simInit(0x1234)
      val sintReg = Reg(SInt(8 bits)).simInit(-5)
      val bitsReg = Reg(Bits(8 bits)).simInit(0xF0)
      val boolReg = Reg(Bool()).simInit(True)
      val minReg = Reg(UInt(8 bits)).simInit(0)
      val maxReg = Reg(UInt(8 bits)).simInit(255)
      val singleBit = Reg(UInt(1 bit)).simInit(1)
      
      val counter8Out = out(counter8)
      val counter16Out = out(counter16)
      val sintRegOut = out(sintReg)
      val bitsRegOut = out(bitsReg)
      val boolRegOut = out(boolReg)
      val minRegOut = out(minReg)
      val maxRegOut = out(maxReg)
      val singleBitOut = out(singleBit)
      
      counter8  := counter8 + 1
      counter16 := counter16 + 1
      sintReg := sintReg + 1
      bitsReg := bitsReg.rotateLeft(1)
      boolReg := !boolReg
      minReg := minReg + 1
      maxReg := maxReg - 1
      singleBit := ~singleBit
    }
    
    // Verify Verilog generation
    val component = SpinalVerilog(new BasicSimInitComponent())
    val verilogContent = scala.io.Source.fromFile(s"./simWorkspace/${component.toplevel.definitionName}.v").mkString
    
    assert(verilogContent.contains("initial begin"))
    assert(verilogContent.contains("counter8 = 8'h42"))
    assert(verilogContent.contains("counter16 = 16'h1234"))
    assert(verilogContent.contains("sintReg = 8'hfb"))
    assert(verilogContent.contains("bitsReg = 8'hf0"))
    assert(verilogContent.contains("boolReg = 1'b1"))
    assert(verilogContent.contains("singleBit = 1'b1"))
    
    // Verify simulation behavior
    SimConfig.doSim(new BasicSimInitComponent) { dut =>
      dut.clockDomain.forkStimulus(10)
      
      def toSignedInt(value: Int, bitWidth: Int): Int = {
        val mask = (1 << bitWidth) - 1
        val maskedValue = value & mask
        if ((maskedValue & (1 << (bitWidth - 1))) != 0) {
          maskedValue - (1 << bitWidth)
        } else {
          maskedValue
        }
      }
      
      // Verify initial values
      assert(dut.counter8Out.toInt == 0x42)
      assert(dut.counter16Out.toInt == 0x1234)
      assert(toSignedInt(dut.sintRegOut.toInt, 8) == -5)
      assert(dut.bitsRegOut.toInt == 0xF0)
      assert(dut.boolRegOut.toBoolean == true)
      assert(dut.minRegOut.toInt == 0)
      assert(dut.maxRegOut.toInt == 255)
      assert(dut.singleBitOut.toInt == 1)
      
      // Verify logic after one clock cycle
      dut.clockDomain.waitRisingEdge()
      sleep(0)
      
      assert(dut.counter8Out.toInt == 0x43)
      assert(dut.counter16Out.toInt == 0x1235)
      assert(toSignedInt(dut.sintRegOut.toInt, 8) == -4)
      assert(dut.bitsRegOut.toInt == 0xE1)
      assert(dut.boolRegOut.toBoolean == false)
      assert(dut.minRegOut.toInt == 1)
      assert(dut.maxRegOut.toInt == 254)
      assert(dut.singleBitOut.toInt == 0)
    }
  }

  test("chained calls and composite types") {
    class TestBundle extends Bundle {
      val field1 = UInt(8 bits)
      val field2 = Bool()
    }
    
    class ChainedComponent extends Component {
      val reg1 = Reg(UInt(8 bits)).init(0x00).simInit(0x55)
      val reg2 = Reg(UInt(8 bits)).simInit(0xAA).init(0xFF)
      
      val bundleReg = Reg(new TestBundle())
      bundleReg.field1.simInit(0x11)
      bundleReg.field2.simInit(False)
      
      val hexReg = Reg(UInt(8 bits)).simInit(0xFF)
      val decReg = Reg(UInt(8 bits)).simInit(42)
      val spinalReg = Reg(UInt(8 bits)).simInit(U(0x33, 8 bits))
      
      val reg1Out = out(reg1)
      val reg2Out = out(reg2)
      val bundleRegOut = out(bundleReg)
      val hexRegOut = out(hexReg)
      val decRegOut = out(decReg)
      val spinalRegOut = out(spinalReg)
      
      reg1 := reg1 + 1
      reg2 := reg2 + 1
      bundleReg.field1 := bundleReg.field1 + 1
      bundleReg.field2 := !bundleReg.field2
      hexReg := hexReg + 1
      decReg := decReg + 1
      spinalReg := spinalReg + 1
    }
    
    // Verify simulation behavior
    SimConfig.doSim(new ChainedComponent) { dut =>
      dut.clockDomain.forkStimulus(10)
      
      // Test simInit values without reset
      assert(dut.reg1Out.toInt == 0x55)
      assert(dut.reg2Out.toInt == 0xAA)
      assert(dut.bundleRegOut.field1.toInt == 0x11)
      assert(dut.bundleRegOut.field2.toBoolean == false)
      assert(dut.hexRegOut.toInt == 0xFF)
      assert(dut.decRegOut.toInt == 42)
      assert(dut.spinalRegOut.toInt == 0x33)
      
      // Test reset behavior
      dut.clockDomain.assertReset()
      dut.clockDomain.waitRisingEdge()
      dut.clockDomain.deassertReset()
      
      assert(dut.reg1Out.toInt == 0x00)
      assert(dut.reg2Out.toInt == 0xFF)
      
      // Test logic after reset
      dut.clockDomain.waitRisingEdge()
      assert(dut.reg1Out.toInt == 0x01)
      assert(dut.reg2Out.toInt == 0x00) // 0xFF + 1 = 0x00 (overflow)
    }
  }

  test("reset type compatibility") {
    // Test 1: Sync reset registers
    class SyncResetComponent extends Component {
      implicit val syncResetConfig = ClockDomainConfig(resetKind = SYNC)
      val reg1 = Reg(UInt(8 bits)).init(0x11).simInit(0xAA)
      val reg2 = Reg(UInt(8 bits)).simInit(0xBB)
      val reg1Out = out(reg1)
      val reg2Out = out(reg2)
      reg1 := reg1 + 1
      reg2 := reg2 + 1
    }
    
    // Test 2: Async reset registers
    class AsyncResetComponent extends Component {
      implicit val asyncResetConfig = ClockDomainConfig(resetKind = ASYNC)
      val reg1 = Reg(UInt(8 bits)).init(0x22).simInit(0xCC)
      val reg2 = Reg(UInt(8 bits)).simInit(0xDD)
      val reg1Out = out(reg1)
      val reg2Out = out(reg2)
      reg1 := reg1 + 1
      reg2 := reg2 + 1
    }
    
    // Test 3: No reset registers
    class NoResetComponent extends Component {
      implicit val noResetConfig = ClockDomainConfig(resetKind = BOOT)
      val reg1 = Reg(UInt(8 bits)).simInit(0xEE)
      val reg2 = Reg(UInt(8 bits)).simInit(0xFF)
      val reg1Out = out(reg1)
      val reg2Out = out(reg2)
      reg1 := reg1 + 1
      reg2 := reg2 + 1
    }
    
    // Verify Verilog generation (quick check)
    val syncComponent = SpinalVerilog(new SyncResetComponent())
    val asyncComponent = SpinalVerilog(new AsyncResetComponent())
    val noResetComponent = SpinalVerilog(new NoResetComponent())
    
    SimConfig.doSim(new SyncResetComponent) { dut =>
      dut.clockDomain.forkStimulus(10)
      assert(dut.reg1Out.toInt == 0xAA)
      assert(dut.reg2Out.toInt == 0xBB)
      
      // Simple reset test
      dut.clockDomain.assertReset()
      dut.clockDomain.waitRisingEdge()
      assert(dut.reg1Out.toInt == 0x11)
    }
    
    SimConfig.doSim(new AsyncResetComponent) { dut =>
      dut.clockDomain.forkStimulus(10)
      assert(dut.reg1Out.toInt == 0xCC)
      assert(dut.reg2Out.toInt == 0xDD)
      
      // Simple reset test
      dut.clockDomain.assertReset()
      sleep(1)
      dut.clockDomain.deassertReset()
      sleep(0)
      assert(dut.reg1Out.toInt == 0x22, s"after async reset should be 0x22, actual is ${dut.reg1Out.toInt}")
    }
    
    SimConfig.doSim(new NoResetComponent) { dut =>
      dut.clockDomain.forkStimulus(10)
      assert(dut.reg1Out.toInt == 0xEE, s"no reset reg1 initial value should be 0xEE, actual is ${dut.reg1Out.toInt}")
      assert(dut.reg2Out.toInt == 0xFF, s"no reset reg2 initial value should be 0xFF, actual is ${dut.reg2Out.toInt}")
      
      // Verify continued operation
      val reg1Before = dut.reg1Out.toInt
      dut.clockDomain.waitRisingEdge()
      sleep(0)
      assert(dut.reg1Out.toInt == ((reg1Before + 1) & 0xFF), "no reset register should continue incrementing")
    }
  }

  test("error conditions") {
    // Test simInit on wire should fail
    assertThrows[java.lang.IllegalArgumentException] {
      SpinalVerilog(new Component {
        val wire = UInt(8 bits)
        wire.simInit(42)
        wire := 0
      })
    }
    
    // Test simInit on input port should fail
    assertThrows[java.lang.IllegalArgumentException] {
      SpinalVerilog(new Component {
        val input = in port UInt(8 bits)
        input.simInit(42)
      })
    }
  }
}
