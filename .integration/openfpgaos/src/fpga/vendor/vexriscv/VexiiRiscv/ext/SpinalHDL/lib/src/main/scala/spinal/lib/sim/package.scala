package spinal.lib

import spinal.core.sim._
import spinal.lib.math.Bcd

package object sim {
  implicit class SimBcdPimper(bt: Bcd) extends SimEquiv {
    type SimEquivT = BigInt

    def maxValue: BigInt = BigInt(10).pow(bt.digitCount) - 1

    def #=(value: BigInt): Unit = {
      require(value >= 0, s"Cannot assign negative value $value to BCD")
      require(value <= maxValue, s"Value $value exceeds BCD capacity (max $maxValue)")
      var remaining = value
      for (i <- 0 until bt.digitCount) {
        bt.digits(i) #= (remaining % 10).toInt
        remaining /= 10
      }
    }
    def #=(value: Int): Unit = this #= BigInt(value)
    def #=(value: Long): Unit = this #= BigInt(value)

    def getSim(): BigInt = {
      var result = BigInt(0)
      var multiplier = BigInt(1)
      for (i <- 0 until bt.digitCount) {
        result += BigInt(bt.digits(i).toInt) * multiplier
        multiplier *= 10
      }
      result
    }
    def toBigInt: BigInt = getSim()
    def toInt: Int = getSim().toInt
    def toLong: Long = getSim().toLong

    def randomize(): BigInt = {
      val value = BigInt(bt.digitCount * 4, simRandom) % (maxValue + 1)
      this #= value
      value
    }

    def toDigits: Seq[Int] = (0 until bt.digitCount).map(i => bt.digits(i).toInt)
    def isValid: Boolean = toDigits.forall(d => d >= 0 && d <= 9)
  }
}
