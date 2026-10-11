package spinal.core.internals

import spinal.core._
import spinal.idslplugin.Location

object ReportFormatting {

  def severityLabel(severity: AssertNodeSeverity): String = severity match {
    case `NOTE`    => "NOTE"
    case `WARNING` => "WARNING"
    case `ERROR`   => "ERROR"
    case `FAILURE` => "FAILURE"
  }

  def fileWithExt(loc: Location): String = {
    val file0 = loc.file
    val file1 = if (file0.startsWith("file:")) file0.stripPrefix("file:") else file0
    val fileWithExt0 =
      if (file1.endsWith(".scala") || file1.endsWith(".sc")) file1
      else file1 + ".scala"
    fileWithExt0.replace('\\', '/')
  }

  def renderPrefix(format: String, loc: Location, severity: AssertNodeSeverity): String = {
    format
      .replace("$FILE", fileWithExt(loc))
      .replace("$LINE", loc.line.toString)
      .replace("$SEVERITY", severityLabel(severity))
  }

  def resolveFormat(statement: AssertStatement, spinalConfig: SpinalConfig): String = {
    statement.getTag(classOf[reportSourceLocationFormatTag]).map(_.format).getOrElse(spinalConfig.reportSourceLocationFormat)
  }
}

