package spinal.lib.misc

import spinal.core._
import spinal.core.fiber._
import spinal.lib._

import scala.collection.mutable
import scala.collection.mutable.ArrayBuffer

sealed trait InterruptMode
object EDGE_RISING extends InterruptMode
object EDGE_FALLING extends InterruptMode
object LEVEL_HIGH extends InterruptMode
object LEVEL_LOW extends InterruptMode
object SPURIOUS extends InterruptMode

trait InterruptCtrlFiber extends Nameable{
  val lock = Lock()

  def defaultInterruptMode: InterruptMode

  def createInterruptMaster(id: Int): InterruptNode
  def createInterruptSlave(id: Int, mode: InterruptMode): InterruptNode

  def createInterruptSlave(id: Int): InterruptNode = createInterruptSlave(id, defaultInterruptMode)

  val mappedInterrupts = mutable.LinkedHashMap[InterruptNode, InterruptNode]()

  def mapUpInterrupt(id: Int, node: InterruptNode): Unit = mapUpInterrupt(id, node, defaultInterruptMode)

  def mapUpInterrupt(id: Int, node: InterruptNode, mode: InterruptMode): Unit = {
    val local = createInterruptSlave(id, mode)
    local.setLambdaName(node.isNamed && this.isNamed)(s"${this.getName()}_from_${node.getName}")
    local << node
    mappedInterrupts(node) = local
  }

  def mapDownInterrupt(id: Int, node: InterruptNode): Unit = {
    val local = createInterruptMaster(id)
    local.setLambdaName(node.isNamed && this.isNamed)(s"${this.getName()}_to_${node.getName}")
    node << local
    mappedInterrupts(node) = local
  }

  def retain() = lock.retain()
  def release() = lock.release()
}

case class ChildInterruptCtrl(intc: InterruptCtrlFiber, filter: (Int, InterruptMode) => Boolean) extends Area {
  var sources = ArrayBuffer[(Int, InterruptNode)]()

  def sourceIds = sources.map(_._1).toSeq
  def interruptNodes = sources.map(_._2).toSeq

  def mapDeletagedInterrupt(id: Int, mode: InterruptMode): Unit = {
    if (filter(id, mode)) {
      val node = InterruptNode.master()
      intc.mapUpInterrupt(id, node, mode)
      sources.addRet((id, node))
    }
  }
}

trait CascadedInterruptCtrlFiber extends InterruptCtrlFiber{
  var childs = ArrayBuffer[ChildInterruptCtrl]()

  def addChildCtrl(intc: InterruptCtrlFiber, filter: (Int, InterruptMode) => Boolean = (_, _) => true): ChildInterruptCtrl = {
    childs.addRet(ChildInterruptCtrl(intc, filter))
  }

  /**
    * This function is the same as createInterruptSlave, but only create interrupt
    * slave for current interrupt controller
    */
  def createInternalInterruptSlave(id: Int, mode: InterruptMode): InterruptNode

  override def createInterruptSlave(id: Int, mode: InterruptMode): InterruptNode = {
    for (child <- childs) {
      child.mapDeletagedInterrupt(id, mode)
    }

    createInternalInterruptSlave(id, mode)
  }
}
