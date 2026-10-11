$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$pkg="$root/build/smoke/ChisBurner-dev.186-gbrtc"
$base="$root/build/smoke/ChisBurner-dev.185-mbc7/SD_ROOT"
$paths=@('Assets/chisburner/common/cb102.elf','Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')
foreach($p in $paths){
 if(!(Test-Path -LiteralPath "D:/$p")){throw "USB missing $p"}
 if((Get-FileHash -LiteralPath "D:/$p").Hash-ne(Get-FileHash -LiteralPath "$base/$p").Hash){throw "USB changed from verified dev185: $p"}
}
foreach($line in Get-Content -LiteralPath "$pkg/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$pkg/SD_ROOT/$p").Hash-ne$hash){throw "Package mismatch $p"}
}
$backup="$root/build/usb-before186-$(Get-Date -Format yyyyMMdd-HHmmss)"
New-Item -ItemType Directory -Path $backup | Out-Null
foreach($p in $paths){
 $target=Join-Path $backup $p
 New-Item -ItemType Directory -Path (Split-Path $target) -Force | Out-Null
 Copy-Item -LiteralPath "D:/$p" -Destination $target
 if((Get-FileHash -LiteralPath $target).Hash-ne(Get-FileHash -LiteralPath "D:/$p").Hash){throw "Rollback copy mismatch $p"}
}
foreach($p in $paths){Copy-Item -LiteralPath "$pkg/SD_ROOT/$p" -Destination "D:/$p" -Force}
foreach($p in $paths){
 if((Get-FileHash -LiteralPath "D:/$p").Hash-ne(Get-FileHash -LiteralPath "$pkg/SD_ROOT/$p").Hash){throw "USB readback mismatch $p"}
}
Write-Host "USB186 PASS: three files replaced and SHA256 readback verified; rollback $backup"
Write-Host 'Saves, ROM files, screenshots, logs and settings untouched.'
