$ErrorActionPreference='Stop'
$tempRoot=Join-Path ([IO.Path]::GetTempPath()) ('chis-eeprom-verify-'+[guid]::NewGuid().ToString('N'))
$null=New-Item -ItemType Directory -Path $tempRoot
$verifier=Join-Path $PSScriptRoot 'verify-eeprom139.ps1'
$checks=0
# Generated test artifacts only. Leave this uniquely named directory for audit.
foreach($size in @(512,8192)){
 $data=New-Object byte[] $size
 for($i=0;$i -lt $size;$i++){$data[$i]=($i*37+11)%256}
 $reference=Join-Path $tempRoot "reference-$size.bin"
 $capture=Join-Path $tempRoot "capture-$size.bin"
 [IO.File]::WriteAllBytes($reference,$data)
 [IO.File]::WriteAllBytes($capture,$data)
 & $verifier -Reference $reference -Capture $capture | Out-Null
 $checks++
 $data[$size-1]=$data[$size-1] -bxor 1
 [IO.File]::WriteAllBytes($capture,$data)
 $rejected=$false
 try { & $verifier -Reference $reference -Capture $capture | Out-Null }
 catch {if($_.Exception.Message -notlike 'FAIL:*'){throw};$rejected=$true}
 if(!$rejected){throw 'Corrupt payload accepted'}
 $checks++
 [IO.File]::WriteAllBytes($capture,[byte[]](1,2,3))
 $rejected=$false
 try { & $verifier -Reference $reference -Capture $capture | Out-Null }
 catch {if($_.Exception.Message -notlike 'Length mismatch:*'){throw};$rejected=$true}
 if(!$rejected){throw 'Truncated payload accepted'}
 $checks++
}
Write-Output "Verifier PASS: $checks checks; artifacts: $tempRoot"
