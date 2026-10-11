param([Parameter(Mandatory)][string]$Path)
$ErrorActionPreference='Stop'
$file=Get-Item -LiteralPath $Path
if($file.PSIsContainer -or $file.Length -ne 65539){throw 'FAIL: expected exactly 65539 bytes'}
$expected='98575d8008feef04461f4fc68338261fe3b9eed590f96a511a914f4d5d504ec3'
$actual=(Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
if($actual -cne $expected){throw "FAIL: SHA256 $actual"}
Write-Host 'PASS: 65539 bytes and SHA-256 match the known SDRAM123 fixture.'
