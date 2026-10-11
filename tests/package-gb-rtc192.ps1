param([int]$Revision=192,[int]$BaseRevision=191)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.$BaseRevision-gbrtc"
$dst="$root/build/smoke/ChisBurner-dev.$Revision-gbrtc"
if(Test-Path -LiteralPath $dst){throw 'Preserve existing package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$p").Hash-ne$hash){throw "Base mismatch $p"}
}
$app="$root/build/gb-rtc$Revision-app/app.elf"
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app)).Contains("dev.$Revision-gbrtc")){throw 'Wrong app'}
New-Item -ItemType Directory -Path $dst | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
Copy-Item -LiteralPath $app -Destination "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
if($Revision -eq 195){
 $manifestPath="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/data.json"
 $manifest=Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
 $slot=$manifest.data.data_slots | Where-Object {$_.id -eq 8}
 if($slot.address-ne'0x20380000'-or$slot.filename-ne'chisburner-ui.cfg'){throw 'Unexpected config contract'}
 $slot.address='0x137C0000'
 [IO.File]::WriteAllText($manifestPath,($manifest | ConvertTo-Json -Depth 30),[Text.UTF8Encoding]::new($false))
}
Copy-Item "$root/docs/GB_RTC${Revision}_TEST_CN.md" "$dst/README_CN.md"
$lines=Get-ChildItem "$dst/SD_ROOT" -File -Recurse | Sort-Object FullName | ForEach-Object {
 $p=$_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/')
 $h=(Get-FileHash -LiteralPath $_.FullName).Hash
 if($p-ne'Assets/chisburner/common/cb102.elf' -and !($Revision-eq195-and$p-eq'Cores/ChisBurner.ChisBurner/data.json') -and $h-ne(Get-FileHash -LiteralPath "$base/SD_ROOT/$p").Hash){throw "Unexpected change $p"}
 "$h  $p"
}
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$lines)
Compress-Archive "$dst/*" "$dst.zip"
Get-FileHash "$dst.zip"
