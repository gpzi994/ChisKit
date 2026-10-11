package spinal.core

import spinal.core.sim._
import spinal.tester.scalatest.SpinalSimFunSuite

class SwitchTest extends Component {
  val io = new Bundle {
    val input = in SInt(3 bits)
    val output = out SInt(3 bits)
  }

  switch (io.input) {
    is (1, 2, 3) {
      io.output := 1
    }
    is (0) {
      io.output := 0
    }
    is (-1, -2, -3) {
      io.output := -1
    }
    default {
      io.output := -2
    }
  }
}

class SpinalSimSwitchTester extends SpinalSimFunSuite {
  test("compile") {
    SimConfig.withFstWave.compile(new SwitchTest)
  }
}
