param(
 [Parameter(Mandatory=$true)][string]$Backup,
 [Parameter(Mandatory=$true)][string]$Reference
)
$ErrorActionPreference='Stop'
$a=Get-Item -LiteralPath $Backup
$b=Get-Item -LiteralPath $Reference
if($a.Length -notin @(32768,65536,131072)){throw "Unsupported backup length: $($a.Length)"}
if($a.Length -ne $b.Length){throw "Size mismatch: backup $($a.Length), reference $($b.Length). Check save profile."}
$x=[IO.File]::ReadAllBytes($a.FullName)
$y=[IO.File]::ReadAllBytes($b.FullName)
for($i=0;$i -lt $x.Length;$i++){
 if($x[$i] -ne $y[$i]){throw ('FAIL first difference at 0x{0:X5}: backup {1:X2}, reference {2:X2}' -f $i,$x[$i],$y[$i])}
}
Write-Output "PASS: all $($x.Length) bytes match. SHA256: $((Get-FileHash -LiteralPath $a.FullName -Algorithm SHA256).Hash)"
