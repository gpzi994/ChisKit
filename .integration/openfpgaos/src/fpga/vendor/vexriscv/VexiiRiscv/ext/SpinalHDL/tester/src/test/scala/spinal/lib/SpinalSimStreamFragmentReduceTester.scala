package spinal.lib

import spinal.core._
import spinal.core.sim._
import spinal.lib.sim._
import spinal.tester.SpinalSimFunSuite

class SpinalSimStreamFragmentReduceTester extends SpinalSimFunSuite {
  test("reduce") {
    SimConfig.compile {
      new Component {
        val input = slave(Stream(Fragment(UInt(8 bits))))
        val output = master(input.reduce(U(0, 8 bits), (next: UInt, acc: UInt, frag: UInt) => next := acc + frag))
      }
    }.doSimUntilVoid { dut =>
      SimTimeout(100000)
      dut.clockDomain.forkStimulus(10)

      val scoreboard = ScoreboardInOrder[Int]()
      var acc = 0
      StreamDriver(dut.input, dut.clockDomain) { p =>
        val v = simRandom.nextInt(256)
        val last = simRandom.nextInt(5) == 0
        p.fragment #= v
        p.last #= last
        acc = (acc + v) & 0xFF
        if (last) { scoreboard.pushRef(acc); acc = 0 }
        true
      }
      StreamReadyRandomizer(dut.output, dut.clockDomain)
      StreamMonitor(dut.output, dut.clockDomain) { p =>
        scoreboard.pushDut(p.toInt)
      }
      dut.clockDomain.onSamplings {
        if (scoreboard.matches >= 100) simSuccess()
      }
    }
  }
}
