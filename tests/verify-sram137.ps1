param(
 [Parameter(Mandatory=$true)][string]$Backup,
 [Parameter(Mandatory=$true)][string]$Reference
)
$ErrorActionPreference='Stop'
foreach($p in @($Backup,$Reference)){
 if((Get-Item -LiteralPath $p).Length -ne 32768){throw "Expected exactly 32768 bytes: $p"}
}
$a=Get-FileHash -LiteralPath $Backup -Algorithm SHA256
$b=Get-FileHash -LiteralPath $Reference -Algorithm SHA256
if($a.Hash -cne $b.Hash){
 $x=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $Backup).Path)
 $y=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $Reference).Path)
 for($i=0;$i -lt $x.Length;$i++){
  if($x[$i] -ne $y[$i]){throw ('FAIL first difference at 0x{0:X4}: backup {1:X2}, reference {2:X2}' -f $i,$x[$i],$y[$i])}
 }
 throw 'SHA256 mismatch'
}
Write-Output "PASS: both files are 32768 bytes and SHA256 matches: $($a.Hash)"
