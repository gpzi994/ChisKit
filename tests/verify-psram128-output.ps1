param([Parameter(Mandatory=$true)][string]$Path)
$ErrorActionPreference='Stop'
$data=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $Path).Path)
if($data.Length -ne 1024){throw "Expected 1024 bytes, got $($data.Length)"}
for($i=0;$i -lt $data.Length;$i++){
 if($data[$i] -ne (($i*37+0xc1)-band 255)){throw "Mismatch at byte $i"}
}
Write-Output 'PASS: all 1024 output bytes match the independent expected pattern.'
