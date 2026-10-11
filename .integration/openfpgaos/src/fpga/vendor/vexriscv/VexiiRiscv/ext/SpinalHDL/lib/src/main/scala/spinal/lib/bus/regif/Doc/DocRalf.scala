package spinal.lib.bus.regif

import spinal.lib._


final case class DocRalf(name : String, backdoor: Boolean = true, flatten: Boolean = false) extends BusIfDoc {
  override val suffix: String = "ralf"
  override def body(): String = {
      s"""#
         |# ${header}
         |# use `ralgen -t top_name -uvm xxx.ralf  -o out/xxx` generate uvm class
         |# Reg Interface Ralf [AUTOGENERATE by SpinalHDL]
         |#
         |block ${this.name} {
         |  endian little;
         |  bytes ${bi.bw};
         |${if(flatten) bi.orderdRegInsts.map(_.toRalf()).mkString("\n") else bi.regSlicesNotReuse.map(_.toRalf()).mkString("\n")};
         |${if(flatten) "" else groupRalf(bi.reuseGroupsById)}
         |}""".stripMargin
  }

  implicit class RegSliceExtend(reg: RegSlice) {
    def toRalf(base: BigInt = 0, tab: String = "", arraySize: Int = 1): String = {
      val arrayStr = if(arraySize == 1) "" else s"[${arraySize}]"
      reg match {
        case ram: RamInst => toRamRalf(ram, base, tab)
        case reg: RegInst => toRegRalf(reg, base, tab, arrayStr)
        case _ => toRegRalf(reg, base, tab, arrayStr)
      }
    }

    def toRamRalf(ram: RamInst, base: BigInt, tab: String = ""): String = {
      s"""${tab}  memory ${ram.upperName()} @'h${(ram.getAddr() - base).toString(16).toUpperCase} {
         |${tab}    size  ${ram.getSize()};
         |${tab}    bits  ${ram.bi.busDataWidth};
         |${tab}    access rw;
         |${tab}  }""".stripMargin
    }

    def toRegRalf(reg: RegSlice, base: BigInt = 0, tab: String = "", arrayStr: String = ""): String = {
        s"""${tab}  register ${reg.upperName()}${arrayStr} @'h${(reg.getAddr() - base).toString(16).toUpperCase} {
           |${reg.getFields().map(_.toRalf(tab + "  ")).mkString("\n")}
           |${tab}  }""".stripMargin
    }
  }

  def groupRalf(lst: Map[String, Map[Int, List[RegSlice]]]) = {
    lst.map{t =>
      val grpName = t._1
      val grps: List[List[RegSlice]] = t._2.toList.sortBy(_._1).map(_._2)

      def grpRalf(grp: List[RegSlice], grpBase: BigInt, grpNum: Int, gap: Int, blockid:String = ""): String = {
        def toRamArrayRalf(ram: RamInst, grpBase: BigInt, tab: String, grpNum: Int): String = {
          s"""  block ${grpName}${blockid}[${grpNum}] @'h${grpBase.hexString()} +${gap} {
             |    bytes ${bi.bw};
             |${ram.toRalf(grpBase, tab="  ")}
             |  }""".stripMargin
        }

        if (grp.size == 1){
          val inst = grp.head
          (inst, grpNum) match {
            case (ram: RamInst, 1) => ram.toRalf(arraySize = 1)
            case (ram: RamInst, _) => toRamArrayRalf(ram, grpBase, tab = "  ", grpNum)
            case (_, _) => inst.toRalf(arraySize = grpNum)
          }
        } else if(grpNum == 1) {
          grp.map(_.toRalf()).mkString("\n")
        } else {
          s"""  block ${grpName}${blockid}[${grpNum}] @'h${grpBase.hexString()} +${gap} {
             |    bytes ${bi.bw};
             |${grp.map(_.toRalf(grpBase, tab = "  ")).mkString("\n")}
             |  }""".stripMargin
        }
      }

      bi.groupConsecutiveBlocks(grps).zipWithIndex.map{ case ((grp, num, gap), i) =>
        val blockid = if(i == 0) "" else s"_${grp.head.reuseTag.instName.toUpperCase()}"
        grpRalf(grp, grp.head.addr, num, gap, blockid)
      }.mkString("\n")

    }.mkString("\n")
  }

  implicit class FieldDescrExtend(fd: Field) {
    def toRalf(tab: String = ""): String = {
      def rename(name: String) = {
        val pre = if(fd.getWidth() == 1) s"${fd.getSection().start}" else s"${fd.getSection().start}_${fd.getSection().end}"
        val name = fd.getAccessType match {
          case AccessType.NA  => s"rsv_${pre}"
          case AccessType.ROV => s"rov_${pre}"
          case _ => fd.getName()
        }
        name
      }

      def access = {
        fd.getAccessType() match{
          case x if fd.uvmBaseAcc.contains(x) => x.toString.toLowerCase()
          case AccessType.NA  => "ro"
          case AccessType.ROV => "ro"
          case _ => "rw"
        }
      }

      def attribute = {
        if(fd.getAccessType() == AccessType.NA){
          s"\n${tab}    attributes {NO_REG_TEST 1};"
        } else ""
      }

      val forhdlpath = if(backdoor && !(fd.getAccessType() == AccessType.NA)) s"(${rename(fd.getName())})" else ""
      s"""${tab}  field ${rename(fd.getName())} $forhdlpath @${fd.getSection().end} {
         |${tab}    bits ${fd.getWidth()};
         |${tab}    access ${access};
         |${tab}    reset ${formatResetValue(fd.getResetValue(), fd.getWidth())};${attribute}
         |${tab}  }""".stripMargin
    }
  }
}
