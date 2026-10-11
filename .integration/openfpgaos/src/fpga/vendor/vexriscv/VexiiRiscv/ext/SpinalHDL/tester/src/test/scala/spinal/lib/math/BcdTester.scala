package spinal.lib.math

import spinal.core._
import spinal.core.sim._
import spinal.lib.sim._
import spinal.tester.SpinalAnyFunSuite
import scala.util.Random

class BcdTester extends SpinalAnyFunSuite {

  test("instantiation") {
    SimConfig.compile(new Component {
      val bcd1 = Bcd(4)
      val bcd2 = Bcd(8)
      assert(bcd1.digitCount == 4 && bcd1.bitWidth == 16)
      assert(bcd2.digitCount == 8 && bcd2.bitWidth == 32)
    })
  }

  test("sim_equiv_basic") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val output = out(Bcd(4))
      }
      io.output := io.input
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 100) {
        val value = Random.nextInt(10000)
        dut.io.input #= value
        sleep(1)
        assert(dut.io.output.toBigInt == value)
      }
      dut.io.input #= 1234; sleep(1)
      assert(dut.io.output.toInt == 1234 && dut.io.output.toLong == 1234L)
      dut.io.input #= 5678; sleep(1)
      assert(dut.io.output.toDigits == Seq(8, 7, 6, 5) && dut.io.output.isValid)
    }
  }

  test("sim_equiv_randomize") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val output = out(Bcd(4))
      }
      io.output := io.input
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 100) {
        val v = dut.io.input.randomize()
        sleep(1)
        assert(v >= 0 && v < 10000 && dut.io.output.toBigInt == v && dut.io.input.isValid)
      }
    }
  }

  test("addition") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val a = in(Bcd(4))
        val b = in(Bcd(4))
        val sum = out(Bcd(5))
      }
      io.sum := (io.a + io.b).resize(5)
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 1000) {
        val a = Random.nextInt(10000)
        val b = Random.nextInt(10000)
        dut.io.a #= a; dut.io.b #= b; sleep(1)
        assert(dut.io.sum.toBigInt == a + b, s"$a + $b")
      }
    }
  }

  test("addition_with_carry") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val a = in(Bcd(4))
        val b = in(Bcd(4))
        val sum = out(Bcd(5))
      }
      io.sum := (io.a + io.b).resize(5)
    }).doSim(seed = 42) { dut =>
      for ((a, b, expected) <- Seq((9999, 1, 10000), (9999, 9999, 19998), (5000, 5000, 10000), (1234, 8766, 10000))) {
        dut.io.a #= a; dut.io.b #= b; sleep(1)
        assert(dut.io.sum.toBigInt == expected, s"$a + $b")
      }
    }
  }

  test("uint_to_bcd_conversion") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in UInt(16 bits)
        val output = out(Bcd(5))
      }
      io.output := Bcd.fromUInt(io.input).resize(5)
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 1000) {
        val value = Random.nextInt(65536)
        dut.io.input #= value; sleep(1)
        assert(dut.io.output.toBigInt == value)
      }
    }
  }

  test("bcd_to_uint_conversion") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val output = out UInt(14 bits)
      }
      io.output := io.input.toUInt.resize(14 bits)
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 1000) {
        val value = Random.nextInt(10000)
        dut.io.input #= value; sleep(1)
        assert(dut.io.output.toInt == value)
      }
    }
  }

  test("left_shift_constant") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val output = out(Bcd(6))
      }
      io.output := io.input << 2
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 100) {
        val value = Random.nextInt(10000)
        dut.io.input #= value; sleep(1)
        assert(dut.io.output.toBigInt == value * 100)
      }
    }
  }

  test("right_shift_constant") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val output = out(Bcd(2))
      }
      io.output := io.input >> 2
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 100) {
        val value = Random.nextInt(10000)
        dut.io.input #= value; sleep(1)
        assert(dut.io.output.toBigInt == value / 100)
      }
    }
  }

  test("comparison") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val a = in(Bcd(4))
        val b = in(Bcd(4))
        val lt, gt, le, ge, eq, ne = out Bool()
      }
      io.lt := io.a < io.b;  io.gt := io.a > io.b
      io.le := io.a <= io.b; io.ge := io.a >= io.b
      io.eq := io.a === io.b; io.ne := io.a =/= io.b
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 1000) {
        val a = Random.nextInt(10000)
        val b = Random.nextInt(10000)
        dut.io.a #= a; dut.io.b #= b; sleep(1)
        assert(dut.io.lt.toBoolean == (a < b) && dut.io.gt.toBoolean == (a > b))
        assert(dut.io.le.toBoolean == (a <= b) && dut.io.ge.toBoolean == (a >= b))
        assert(dut.io.eq.toBoolean == (a == b) && dut.io.ne.toBoolean == (a != b))
      }
    }
  }

  test("resize") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val expand = out(Bcd(6))
        val shrink = out(Bcd(2))
      }
      io.expand := io.input.resize(6)
      io.shrink := io.input.resize(2)
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 100) {
        val value = Random.nextInt(10000)
        dut.io.input #= value; sleep(1)
        assert(dut.io.expand.toBigInt == value && dut.io.shrink.toBigInt == value % 100)
      }
    }
  }

  test("leading_zeroes") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val leadingZeroes = out UInt(3 bits)
        val usedDigits = out UInt(3 bits)
      }
      io.leadingZeroes := io.input.leadingZeroes.resize(3 bits)
      io.usedDigits := io.input.usedDigits.resize(3 bits)
    }).doSim(seed = 42) { dut =>
      for ((value, leading, used) <- Seq((0,4,0), (1,3,1), (10,2,2), (100,1,3), (1000,0,4), (123,1,3), (9999,0,4))) {
        dut.io.input #= value; sleep(1)
        assert(dut.io.leadingZeroes.toInt == leading && dut.io.usedDigits.toInt == used)
      }
    }
  }

  test("is_zero") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val isZero = out Bool()
      }
      io.isZero := io.input.isZero
    }).doSim(seed = 42) { dut =>
      dut.io.input #= 0; sleep(1)
      assert(dut.io.isZero.toBoolean)
      for (_ <- 0 until 100) {
        dut.io.input #= Random.nextInt(9999) + 1; sleep(1)
        assert(!dut.io.isZero.toBoolean)
      }
    }
  }

  test("concatenation") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val high = in(Bcd(2))
        val low = in(Bcd(2))
        val result = out(Bcd(4))
      }
      io.result := io.high @@ io.low
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 100) {
        val high = Random.nextInt(100)
        val low = Random.nextInt(100)
        dut.io.high #= high; dut.io.low #= low; sleep(1)
        assert(dut.io.result.toBigInt == high * 100 + low)
      }
    }
  }

  test("lsd_msd") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val lsd = out UInt(4 bits)
        val msd = out UInt(4 bits)
      }
      io.lsd := io.input.lsd
      io.msd := io.input.msd
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 100) {
        val value = Random.nextInt(10000)
        dut.io.input #= value; sleep(1)
        assert(dut.io.lsd.toInt == value % 10 && dut.io.msd.toInt == (value / 1000) % 10)
      }
    }
  }

  test("slice_extraction") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(6))
        val offset = in UInt(3 bits)
        val slice = out(Bcd(2))
      }
      io.slice := io.input(io.offset, 2)
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 100) {
        val value = Random.nextInt(1000000)
        val offset = Random.nextInt(5)
        dut.io.input #= value; dut.io.offset #= offset; sleep(1)
        assert(dut.io.slice.toBigInt == (value / BigInt(10).pow(offset).toInt) % 100)
      }
    }
  }

  test("roundtrip_uint") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in UInt(12 bits)
        val output = out UInt(14 bits)
      }
      io.output := Bcd.fromUInt(io.input).toUInt.resize(14 bits)
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 1000) {
        val value = Random.nextInt(4096)
        dut.io.input #= value; sleep(1)
        assert(dut.io.output.toInt == value)
      }
    }
  }

  test("left_shift_variable") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(4))
        val shift = in UInt(3 bits)
        val output = out(Bcd(11))
      }
      io.output := (io.input << io.shift).resize(11)
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 500) {
        val value = Random.nextInt(10000)
        val shift = Random.nextInt(8)
        dut.io.input #= value; dut.io.shift #= shift; sleep(1)
        assert(dut.io.output.toBigInt == value * BigInt(10).pow(shift))
      }
    }
  }

  test("right_shift_variable") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val input = in(Bcd(6))
        val shift = in UInt(3 bits)
        val output = out(Bcd(6))
      }
      io.output := io.input >> io.shift
    }).doSim(seed = 42) { dut =>
      for (_ <- 0 until 500) {
        val value = Random.nextInt(1000000)
        val shift = Random.nextInt(7)
        dut.io.input #= value; dut.io.shift #= shift; sleep(1)
        assert(dut.io.output.toBigInt == value / BigInt(10).pow(shift).toInt)
      }
    }
  }

  test("comparison_mismatched_lengths") {
    SimConfig.compile(new Component {
      val io = new Bundle {
        val small = in(Bcd(2))
        val large = in(Bcd(4))
        val eq, ne, lt, gt = out Bool()
      }
      io.eq := io.small === io.large; io.ne := io.small =/= io.large
      io.lt := io.small < io.large;   io.gt := io.small > io.large
    }).doSim(seed = 42) { dut =>
      dut.io.small #= 42; dut.io.large #= 42; sleep(1)
      assert(dut.io.eq.toBoolean && !dut.io.ne.toBoolean && !dut.io.lt.toBoolean && !dut.io.gt.toBoolean)

      dut.io.small #= 99; dut.io.large #= 1234; sleep(1)
      assert(!dut.io.eq.toBoolean && dut.io.ne.toBoolean && dut.io.lt.toBoolean && !dut.io.gt.toBoolean)

      for (_ <- 0 until 100) {
        val s = Random.nextInt(100)
        val l = Random.nextInt(10000)
        dut.io.small #= s; dut.io.large #= l; sleep(1)
        assert(dut.io.eq.toBoolean == (s == l) && dut.io.lt.toBoolean == (s < l) && dut.io.gt.toBoolean == (s > l))
      }
    }
  }
}
