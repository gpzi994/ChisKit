[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$RomPath='C:/Users/lin13/Desktop/game/roms/mengjiandao.gbc',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-gb-mbc5-warmup90-s3-win'
)

$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$Path){(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()}
function Wsl([string]$Command){
    & wsl.exe -d $Distro -- bash -lc $Command
    if($LASTEXITCODE -ne 0){throw "Build/test failed: $Command"}
}

$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.86-gb-mbc5-fullrom-sha256'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.90-gb-mbc5-first-read-discard'
$zip="$bundle.zip"
$rom=[IO.Path]::GetFullPath($RomPath)
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'
$fpgaManifest=Join-Path $fpga 'GB_MBC5_WARMUP90_SHA256.txt'
$qsf=Join-Path $fpga 'ap_core.qsf'
$sta=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$appRoot=Join-Path $root 'app/gb_mbc5_warmup90'
$locatorRoot=Join-Path $root 'app/gb_mbc5_bank0loc88'
$app=Join-Path $root 'build/gb-mbc5-warmup90-app/app.elf'
$gbRtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_fullrom.v'
$gbaReadRtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v'
$gbaProgramRtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v'
$utf8=[Text.UTF8Encoding]::new($false)

if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
    throw 'Preserve existing smoke.90 package'
}
foreach($path in @($baseBundle,$base,$rom,$rawRbf,$fpgaManifest,$qsf,$sta,
                   $appRoot,$locatorRoot,$gbRtl,$gbaReadRtl,$gbaProgramRtl)){
    if(!(Test-Path -LiteralPath $path)){throw "Missing package input: $path"}
}
if((Get-Item -LiteralPath $rom).Length -ne 2097152 -or
   (Sha $rom) -ne '8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7'){
    throw 'Wrong MBC5 reference ROM'
}

foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw "Malformed smoke.86 sum: $line"}
    $path=Join-Path $base $Matches[2]
    if(!(Test-Path -LiteralPath $path) -or (Sha $path) -cne $Matches[1]){
        throw "smoke.86 package drift: $($Matches[2])"
    }
}
foreach($line in Get-Content -LiteralPath $fpgaManifest){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw "Malformed FPGA sum: $line"}
    if(!(Test-Path -LiteralPath $Matches[2]) -or (Sha $Matches[2]) -cne $Matches[1]){
        throw "FPGA source drift: $($Matches[2])"
    }
}

$qsfText=Get-Content -Raw -LiteralPath $qsf
if($qsfText -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MBC5_WARMUP90\s*$' -or
   $qsfText -match '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$'){
    throw 'FPGA build is not isolated warmup90'
}
$staText=Get-Content -Raw -LiteralPath $sta
$slacks=@{}
foreach($kind in @('setup','hold')){
    $matches=[regex]::Matches($staText,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if(!$matches.Count){throw "Missing $kind timing result"}
    $value=($matches|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
    if($value -le 0){throw "Refuse package: $kind slack $value ns"}
    $slacks[$kind]=$value
}

$baseOs=Join-Path $base 'Assets/chisburner/common/os.bin'
$baseOsHash=Sha $baseOs
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($baseOs))
$appSource=(Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c'))+
           (Get-Content -Raw -LiteralPath (Join-Path $locatorRoot 'main.c'))+
           (Get-Content -Raw -LiteralPath (Join-Path $locatorRoot 'chunk_crc32.c'))
if($appSource -match 'of_cart_(write|erase|program)'){
    throw 'warmup90 app contains cartridge mutation call'
}
$gbSource=Get-Content -Raw -LiteralPath $gbRtl
if($gbSource -notmatch 'DISCARD_FIRST_BLOCK' -or
   $gbSource -notmatch 'discarding_first_block' -or
   $gbSource -notmatch 'read_command_count < 17''d65536' -or
   $gbSource -notmatch 'address_reg == expected_address'){
    throw 'First-read discard or exact full-ROM guard missing'
}

Wsl "cd '$wroot' && cc -O2 -Wall -Wextra tests/test-bank0-chunkcrc88.c app/gb_mbc5_bank0loc88/chunk_crc32.c -o /tmp/test-bank0crc90 && /tmp/test-bank0crc90 '/mnt/c/Users/lin13/Desktop/game/roms/mengjiandao.gbc'"
Wsl "cd '$wroot/app/gb_mbc5_warmup90' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && rm -rf build/gb-warmup90-package-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gb_mbc5_fullrom -GADDRESS_SETUP_CYCLES=1 -GSELECT_SETUP_CYCLES=1 -GDATA_WAIT_CYCLES=1 -GRELEASE_CYCLES=1 -GWRITE_SETUP_CYCLES=1 -GWRITE_LOW_CYCLES=1 -GWRITE_RELEASE_CYCLES=1 -GDISCARD_FIRST_BLOCK=1 -CFLAGS -DEXPECT_FIRST_BLOCK_DISCARD -Mdir build/gb-warmup90-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_fullrom.v '$wroot/tests/test-gb-mbc5-fullrom86-rtl.cpp' >/dev/null && build/gb-warmup90-package-rtl/Vchisburner_cart_gb_mbc5_fullrom"
Wsl "cd '$wroot' && rm -rf build/gba-warmup90-package-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gba_s29gl256 -Mdir build/gba-warmup90-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v '$wroot/tests/test-program65-rtl.cpp' >/dev/null && build/gba-warmup90-package-rtl/Vchisburner_cart_gba_s29gl256"
if(!(Test-Path -LiteralPath $app)){throw 'MBC5 warmup90 app build missing'}

if(-not ('ChisBurner.Gb90.Reverse' -as [type])){
    Add-Type -TypeDefinition @'
namespace ChisBurner.Gb90 {
    public static class Reverse {
        public static byte[] Apply(byte[] input) {
            var table = new byte[256];
            for (int i=0;i<256;i++) { int x=i,y=0; for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;} table[i]=(byte)y; }
            var output=new byte[input.Length];
            for(int i=0;i<input.Length;i++) output[i]=table[input[i]];
            return output;
        }
    }
}
'@
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($file in Get-ChildItem -LiteralPath $base -Recurse -File){
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if($rel -in @('Assets/chisburner/common/chisburner86.elf',
                  'Assets/chisburner/common/chisburner86.ini',
                  'Cores/ChisBurner.ChisBurner/chis.rbf_r')){continue}
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner90.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner90.ini'),"[os]`nELF=chisburner90.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),
    [ChisBurner.Gb90.Reverse]::Apply([IO.File]::ReadAllBytes($rawRbf)))

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.description='GB/GBC MBC5 first-transaction discard diagnostic'
$core.core.metadata.version='0.1.0-smoke.90-gb-mbc5-first-read-discard'
$core.core.metadata.date_release='2026-09-06'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){throw 'Play Cartridge mode lost'}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='chisburner90.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='chisburner90.elf'
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    'smoke.90: FPGA-local discard of first GB/GBC 32-byte transaction, then exact reread; GBA unchanged; read-only.', $utf8)

if((Sha (Join-Path $common 'os.bin')) -ne $baseOsHash){throw 'Reused smoke.86 OS changed'}
$readme=@"
ChisBurner - smoke.90 GB/GBC 首次总线读取丢弃测试

smoke.88 和 smoke.89 均证明：完整 2 MiB 中只有第一次总线事务 0x0000-0x001F
异常，且实际内容每次变化；应用等待 1000 ms 无效。本版只改变 GB/GBC FPGA 读取状态机：
主机第一个正式 32 字节请求到达后，FPGA 先完整读取同一地址一次并丢弃，再经过正常
/RD 释放周期从 0x0000 重读 32 字节，把第二遍数据返回给应用。

主机仍严格发送 65536 个、按 32 字节递增的正式请求；只有首个请求在 FPGA 内部产生
额外 32 次只读采样。MBC5 Bank 选择仍只可写 0x2000；不存在 SRAM、NOR 擦除或烧写。
GBA 读写模块未修改，并已重新通过 program81 RTL 全回归。

把 SD_ROOT 合并到 SD 卡根目录并覆盖旧文件，关机插入同一张 MBC5 卡后启动
Play Cartridge。合格标准：bank0 CRC32 PASS d0a2a03f、32-byte chunks PASS、
BAD CHUNKS: 0。建议冷启动测试两次；请发送最终完整画面。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST90_CN.txt'),$readme,$utf8)

$provenance=@"
version=0.1.0-smoke.90-gb-mbc5-first-read-discard
quartus_full_compile=PASS
quartus_seed=3
minimum_setup_slack_ns=$($slacks.setup)
minimum_hold_slack_ns=$($slacks.hold)
base_os_exact_reuse=YES
os_sha256=$baseOsHash
first_host_block_physical_reads=64
first_discarded_bytes=32
host_read_commands=65536_EXACT_ASCENDING
reference_rom_sha256=8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
reference_rom_included=NO
mapper_write_address=0x2000_ONLY
save_access=ABSENT
nor_erase_program=ABSENT
gba_program81_rtl_regression=PASS
gba_readonly_source_sha256=$(Sha $gbaReadRtl)
gba_programmer_source_sha256=$(Sha $gbaProgramRtl)
rbf_sha256=$(Sha $rawRbf)
rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
app_sha256=$(Sha (Join-Path $common 'chisburner90.elf'))
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)

Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File|ForEach-Object{
    $null=Get-Content -Raw -LiteralPath $_.FullName|ConvertFrom-Json
}
if(Get-ChildItem -LiteralPath $sd -Recurse -File|Where-Object {$_.Extension -in @('.gb','.gbc','.gba')}){
    throw 'ROM image unexpectedly included in package'
}
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
    "$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"
}
if($sums.Count -ne 14){throw "Unexpected SD file count: $($sums.Count)"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
"MBC5 warmup90 package ready: $zip"
"ZIP SHA256: $(Sha $zip)"
