$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$pub="$root/build/publish/ChisKit-0.01b"
$src="$pub/Source"
$base="$root/build/releases/ChisKit-0.01b-splash209c-test/SD_ROOT"
$sd="$pub/Firmware/SD_ROOT"
foreach($p in @('Platforms/_images/chiskit.bin','Cores/Alius.chiskit/icon.bin')){
 $target=Join-Path $sd $p
 New-Item -ItemType Directory -Path (Split-Path $target) -Force | Out-Null
 Copy-Item -LiteralPath "$root/platform/openfpga/$p" -Destination $target -Force
}
foreach($p in @('Assets/chiskit/common/os.bin','Cores/Alius.chiskit/chiskit.rbf_r')){
 if((Get-FileHash "$sd/$p").Hash-ne(Get-FileHash "$base/$p").Hash){throw "Hardware baseline changed: $p"}
}
foreach($p in @('build/gb-rtcrelease001b-app/app.elf','.integration/openfpgaos/src/firmware/os/bld/chiskit-splash209c/os.bin','.integration/openfpgaos/src/firmware/os/bld/chiskit-splash209c/firmware.mif')){
 if((Get-FileHash "$root/$p").Hash-ne(Get-FileHash "$src/$p").Hash){throw "Independent rebuild differs: $p"}
}
if((Get-FileHash "$sd/Assets/chiskit/common/chiskit.elf").Hash-ne(Get-FileHash "$src/build/gb-rtcrelease001b-app/app.elf").Hash){throw 'ELF mismatch'}
. "$root/tests/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$sd/Assets/chiskit/common/os.bin"))
Copy-Item "$pub/README_CN.md" "$pub/Firmware/README_CN.md"
foreach($tree in @('Firmware','Source')){
 $baseTree="$pub/$tree"
 # Filter relative paths because the workspace itself lives under build.
 $lines=Get-ChildItem $baseTree -File -Recurse -Force | Sort-Object FullName | ForEach-Object {
  $rel=$_.FullName.Substring($baseTree.Length+1).Replace('\','/')
  if($rel -notmatch '(^|/)(build|bld)/' -and $rel-ne'SHA256SUMS.txt'){
   "$( (Get-FileHash -LiteralPath $_.FullName).Hash)  $rel"
  }
 }
 [IO.File]::WriteAllLines("$baseTree/SHA256SUMS.txt",[string[]]$lines)
}
Push-Location $pub
try{
 & tar.exe -a -cf ChisKit-0.01b-firmware.zip -C Firmware/SD_ROOT Cores Assets Platforms Saves
 if($LASTEXITCODE){throw 'Firmware archive failed'}
 & tar.exe -a -cf ChisKit-0.01b-source.zip --exclude=build --exclude=bld Source
 if($LASTEXITCODE){throw 'Source archive failed'}
 $hashes=@('ChisKit-0.01b-firmware.zip','ChisKit-0.01b-source.zip') | ForEach-Object {"$((Get-FileHash $_).Hash)  $_"}
 [IO.File]::WriteAllLines("$pub/SHA256SUMS.txt",[string[]]$hashes)
}finally{Pop-Location}
Write-Host 'PASS: independent app/OS/MIF rebuild byte-identical; tested splash209c pair preserved; root-layout archives created.'
