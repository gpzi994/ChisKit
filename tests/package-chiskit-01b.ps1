$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.199-gbrtc"
$dst="$root/build/releases/chiskit-0.1b"
$fit="$root/build/gb-rtc200-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gb-rtc200"
$app="$root/build/gb-rtc200-app/app.elf"
if(Test-Path $dst){throw 'Preserve existing release'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$p").Hash-ne$hash){throw "Base mismatch $p"}
}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fit -AllowUnusedBridgeReady
if((Get-FileHash "$os/firmware.mif").Hash-ne(Get-FileHash "$fit/firmware.mif").Hash){throw 'MIF mismatch'}
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
$elf=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
if(!$elf.Contains('0.1b')-or !$elf.Contains('/Saves/chiskit/common/')){throw 'Wrong application'}
$core="$dst/SD_ROOT/Cores/Alius.chiskit"
$assets="$dst/SD_ROOT/Assets/chiskit/common"
foreach($d in @($core,$assets,"$dst/SD_ROOT/Platforms","$dst/SD_ROOT/Saves/chiskit/common")){New-Item -ItemType Directory -Path $d -Force | Out-Null}
Get-ChildItem "$base/SD_ROOT/Cores/ChisBurner.ChisBurner" -File | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination $core}
function WriteJson($path,$value){[IO.File]::WriteAllText($path,($value|ConvertTo-Json -Depth 30),[Text.UTF8Encoding]::new($false))}
$c=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$c.core.metadata.platform_ids=@('chiskit');$c.core.metadata.shortname='chiskit';$c.core.metadata.author='Alius'
$c.core.metadata.version='0.1b';$c.core.metadata.date_release='2026-10-10'
$c.core.metadata.description='GB, GBC and GBA cartridge ROM, save and RTC tools.'
$c.core.cores[0].name='chiskit';$c.core.cores[0].filename='chiskit.rbf_r'
WriteJson "$core/core.json" $c
$data=Get-Content "$core/data.json" -Raw|ConvertFrom-Json
foreach($s in $data.data.data_slots){
 if($s.id-eq2){$s.filename='chiskit.ini'}
 if($s.id-eq3){$s.filename='chiskit.elf'}
 if($s.id-eq6){$s.name='Saves SAV'}
 if($s.id-eq20){$s.name='Saves ROM'}
}
WriteJson "$core/data.json" $data
$platform=Get-Content "$base/SD_ROOT/Platforms/chisburner.json" -Raw|ConvertFrom-Json
$platform.platform.name='chiskit';$platform.platform.manufacturer='Alius'
WriteJson "$dst/SD_ROOT/Platforms/chiskit.json" $platform
Copy-Item $app "$assets/chiskit.elf"
Copy-Item "$os/os.bin" "$assets/os.bin"
[IO.File]::WriteAllText("$assets/chiskit.ini","[os]`nELF=chiskit.elf`nARGS=`n")
[IO.File]::WriteAllText("$core/info.txt","chiskit 0.1b`nDeveloped by Alius`nGB/GBC/GBA cartridge tools. Back up before writing.`n")
[IO.File]::WriteAllText("$dst/SD_ROOT/Saves/chiskit/common/DIRECTORY.txt","chiskit backup output directory. Preserve existing backups.`n")
Add-Type -TypeDefinition 'public static class ChiskitReverse {public static byte[] Run(byte[] a){for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int k=0;k<8;k++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;}}'
[IO.File]::WriteAllBytes("$core/chiskit.rbf_r",[ChiskitReverse]::Run([IO.File]::ReadAllBytes("$fit/output_files/ap_core.rbf")))
# Remove only the copied obsolete bitstream in this new, fixed release path.
Remove-Item -LiteralPath "$root/build/releases/chiskit-0.1b/SD_ROOT/Cores/Alius.chiskit/chis.rbf_r"
Copy-Item "$root/docs/CHISKIT_01B_CN.md" "$dst/README_CN.md"
$lines=Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object {
 $p=$_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/')
 if($p-match '(?i)chisburner|cb10'){throw "Old public path $p"}
 "{0}  {1}" -f (Get-FileHash -LiteralPath $_.FullName).Hash,$p
}
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$lines)
Compress-Archive "$dst/*" "$dst.zip"
Get-FileHash "$dst.zip"
