$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$dest=Join-Path $root 'build/smoke/ChisBurner-138-r2-ui-update'
if(Test-Path $dest){throw 'Output already exists; preserve prior package'}
$assets=Join-Path $dest 'SD_ROOT/Assets/chisburner/common'
New-Item -ItemType Directory -Path $assets -Force | Out-Null
$app=Join-Path $root 'build/sram138-r2-app/app.elf'
foreach($name in @('cb101.elf','cb102.elf')){
 Copy-Item -LiteralPath $app -Destination (Join-Path $assets $name)
 if((Get-FileHash $app).Hash -ne (Get-FileHash (Join-Path $assets $name)).Hash){throw 'Copy mismatch'}
}
Copy-Item -LiteralPath (Join-Path $root 'docs/UI138_R2_TEST.md') -Destination (Join-Path $dest 'README.md')
Compress-Archive -Path "$dest/*" -DestinationPath "$dest.zip"
Get-FileHash "$dest.zip"
