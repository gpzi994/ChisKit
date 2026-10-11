$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
$baseline='build/smoke/ChisBurner-smoke.128-sdram-file-save-r8'
$bundle='build/smoke/ChisBurner-smoke.129-dual-psram-cache-r1'
$fpga='build/psram129-r1-seed2/quartus'
$os='.integration/openfpgaos/src/firmware/os/bld/psram129'
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing PSRAM129 package'}
foreach($p in @("$fpga/output_files/ap_core.rbf","$fpga/firmware.mif","$os/firmware.mif","$os/os.bin","$os/firmware.elf")){
 if(!(Test-Path -LiteralPath $p)){throw "Missing build input: $p"}
}
if((Sha "$fpga/firmware.mif") -cne (Sha "$os/firmware.mif")){throw 'BRAM firmware mismatch'}
if((Get-Content "$fpga/full-compile.log" -Raw) -notmatch 'Full Compilation was successful. 0 errors'){throw 'Full FPGA compile did not succeed'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot "$root/$fpga"
. "$PSScriptRoot/os-image-integrity.ps1"
$bytes=[IO.File]::ReadAllBytes("$root/$os/os.bin")
$text=[Text.Encoding]::ASCII.GetString($bytes)
foreach($needle in @('ChisBurner smoke.129-r1','PSRAM0 load PASS','PSRAM1 load PASS','Full 32 MiB CRC PASS','No cartridge/NOR operations.')){
 if(!$text.Contains($needle)){throw "Missing diagnostic marker: $needle"}
}
$null=Assert-ChisBurnerOsIntegrity -Bytes $bytes
$utf8=[Text.UTF8Encoding]::new($false)
function Json($p,$o){[IO.File]::WriteAllText("$root/$p",($o|ConvertTo-Json -Depth 20),$utf8)}
if(-not('ChisBurner.Ps129.Util'-as[type])){Add-Type -TypeDefinition @'
using System;
using System.IO;
namespace ChisBurner.Ps129 {
 public static class Util {
  public static byte[] Reverse(byte[] a){var t=new byte[256];for(int i=0;i<256;i++){int x=i,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[i]=(byte)y;}var r=new byte[a.Length];for(int i=0;i<a.Length;i++)r[i]=t[a[i]];return r;}
  static uint Step(uint c,byte b){c^=b;for(int k=0;k<8;k++)c=(c&1)!=0?(c>>1)^0xedb88320u:c>>1;return c;}
  public static string Fixture(string path){const int total=0x2000000,chunk=0x100000;uint c0=0xffffffffu,c1=0xffffffffu;var buf=new byte[chunk];using(var f=new FileStream(path,FileMode.CreateNew,FileAccess.Write,FileShare.None)){for(int baseOff=0;baseOff<total;baseOff+=chunk){for(int j=0;j<chunk;j++){uint i=(uint)(baseOff+j);byte b=(byte)((i*37u+((i>>8)*13u)+((i>>16)*7u)+(i<0x1000000u?0x39u:0xa5u))&255u);buf[j]=b;if(i<0x1000000u)c0=Step(c0,b);else c1=Step(c1,b);}f.Write(buf,0,buf.Length);}}return String.Format("{0:X8} {1:X8}",~c0,~c1);}
 }
}
'@}
$core="$bundle/SD_ROOT/Cores/ChisBurner.PSRAM129"
$assets="$bundle/SD_ROOT/Assets/chisburnerps129/common"
foreach($dir in @($core,$assets,"$bundle/SD_ROOT/Platforms")){New-Item -ItemType Directory -Path $dir -Force|Out-Null}
$oldcore="$baseline/SD_ROOT/Cores/ChisBurner.PSRAM128"
foreach($name in @('audio.json','video.json','input.json','interact.json','variants.json','loader.bin')){Copy-Item "$oldcore/$name" "$core/$name"}
$meta=Get-Content "$oldcore/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.platform_ids=@('chisburnerps129')
$meta.core.metadata.shortname='PSRAM129'
$meta.core.metadata.description='ChisBurner 32 MiB dual-PSRAM ROM cache load and full-readback diagnostic.'
$meta.core.metadata.version='0.1.0-smoke.129-dual-psram-cache-r1'
$meta.core.metadata.date_release='2026-09-13'
Json "$core/core.json" $meta
$data=[ordered]@{data=[ordered]@{magic='APF_VER_1';data_slots=@(
 [ordered]@{id=0;name='Diagnostic';required=$false;parameters=275;extensions=@('json')},
 [ordered]@{id=1;name='OS Binary';required=$false;parameters=0;extensions=@('bin');deferload=$true;filename='os.bin'},
 [ordered]@{id=6;name='32 MiB cache fixture';required=$false;parameters=0;extensions=@('bin');deferload=$true;filename='psram129-input.bin';size_exact=33554432},
 [ordered]@{id=8;name='SDRAM config';required=$false;parameters='0x84';extensions=@('cfg','bin');deferload=$true;filename='sdram128-config.bin';size_maximum='0x40000'},
 [ordered]@{id=10;name='SDRAM save';required=$false;parameters='0x84';extensions=@('sav','bin');deferload=$true;filename='sdram128-save.bin';size_maximum='0x40000'}
)}}
Json "$core/data.json" $data
$platform=Get-Content "$baseline/SD_ROOT/Platforms/chisburnerps128.json" -Raw|ConvertFrom-Json
$platform.platform.name='ChisBurner PSRAM129'
Json "$bundle/SD_ROOT/Platforms/chisburnerps129.json" $platform
[IO.File]::WriteAllBytes("$root/$core/chis.rbf_r",[ChisBurner.Ps129.Util]::Reverse([IO.File]::ReadAllBytes("$root/$fpga/output_files/ap_core.rbf")))
Copy-Item "$os/os.bin" "$assets/os.bin"
Copy-Item "$baseline/SD_ROOT/Assets/chisburnerps128/common/sdram128-config.bin" "$assets/sdram128-config.bin"
Copy-Item "$baseline/SD_ROOT/Assets/chisburnerps128/common/sdram128-save.bin" "$assets/sdram128-save.bin"
$crc=[ChisBurner.Ps129.Util]::Fixture("$root/$assets/psram129-input.bin")
if($crc -cne '475EF0F6 B5E23C21'){throw "Fixture CRC mismatch: $crc"}
[IO.File]::WriteAllText("$root/$core/info.txt",'smoke.129: TF to PSRAM0+PSRAM1, 32 MiB full hardware readback CRC. No cartridge/NOR operation. Read TEST_CN.md.',$utf8)
Copy-Item docs/SMOKE129_TEST_CN.md "$bundle/TEST_CN.md"
$receipt=@()
foreach($p in @("$os/firmware.elf","$os/firmware.mif","$os/os.bin","$fpga/output_files/ap_core.rbf","$fpga/ap_core.qsf","$fpga/full-compile.log","$fpga/cdc-report.log",'build/psram129/sections.txt','build/psram129/symbols.txt','build/psram129/disassembly.txt')){$receipt+="$(Sha $p)  $p"}
[IO.File]::WriteAllLines("$root/$bundle/BUILD_INPUTS_SHA256.txt",[string[]]$receipt,$utf8)
$sources=@('tests/build-psram129-os.sh','tests/build-psram129-fpga.ps1','tests/configure-psram129.tcl','tests/build-psram129-package.ps1','docs/SMOKE129_TEST_CN.md','core/rtl/chisburner_psram_cache129.v','.integration/openfpgaos/src/fpga/targets/pocket/core_top.v','.integration/openfpgaos/src/fpga/common/axi_periph_slave.v','.integration/openfpgaos/src/firmware/os/targets/pocket/file.c','.integration/openfpgaos/src/firmware/os/hal/file.h','.integration/openfpgaos/src/firmware/os/hal/regs.h','.integration/openfpgaos/src/firmware/os/kernel/main.c')
[IO.File]::WriteAllLines("$root/$bundle/SOURCES_SHA256.txt",[string[]]@($sources|ForEach-Object{"$(Sha $_)  $_"}),$utf8)
$prefix=[IO.Path]::GetFullPath("$bundle/SD_ROOT").TrimEnd('\')+'\'
$sums=@(Get-ChildItem "$bundle/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$root/$bundle/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip"
Get-FileHash "$bundle.zip" -Algorithm SHA256
