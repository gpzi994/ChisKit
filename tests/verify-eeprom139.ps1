param(
 [Parameter(Mandatory=$true)][string]$Reference,
 [Parameter(Mandatory=$true)][string]$Capture
)
$ErrorActionPreference='Stop'
# Read-only verifier: never pad, trim, endian-swap or modify either input.
$referencePath=(Resolve-Path -LiteralPath $Reference).Path
$capturePath=(Resolve-Path -LiteralPath $Capture).Path
$expected=[IO.File]::ReadAllBytes($referencePath)
$actual=[IO.File]::ReadAllBytes($capturePath)
if($expected.Length -notin @(512,8192)){throw 'Reference must be exactly 512 or 8192 bytes.'}
if($actual.Length -ne $expected.Length){throw "Length mismatch: expected $($expected.Length), got $($actual.Length). No truncation permitted."}
$count=0
$first=-1
for($i=0;$i -lt $expected.Length;$i++){
 if($expected[$i] -ne $actual[$i]){if($first -lt 0){$first=$i};$count++}
}
$uniform=$true
for($i=1;$i -lt $expected.Length;$i++){if($expected[$i] -ne $expected[0]){$uniform=$false;break}}
if($uniform){Write-Warning 'Uniform reference: byte equality alone is weak evidence of working serial communication.'}
$refHash=(Get-FileHash -LiteralPath $referencePath -Algorithm SHA256).Hash
$capHash=(Get-FileHash -LiteralPath $capturePath -Algorithm SHA256).Hash
Write-Output "Reference SHA256: $refHash"
Write-Output "Capture   SHA256: $capHash"
if($count){
 throw ('FAIL: {0} differing bytes; first offset 0x{1:X}, expected 0x{2:X2}, got 0x{3:X2}.' -f $count,$first,$expected[$first],$actual[$first])
}
Write-Output "PASS: $($expected.Length) / $($expected.Length) bytes equal. Applies only to this captured file and reference."
