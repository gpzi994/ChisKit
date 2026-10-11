[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$RomPath='C:/Users/lin13/Desktop/game/逆转裁判.gba',
    [string]$OsBuild='.integration/openfpgaos/src/firmware/os/bld/program81b-pocket-release',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-program65-s1-route-maxfit-buf51281m10k-win'
)

$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')

function Sha([string]$Path) {
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}
function Wsl([string]$Command) {
    & wsl.exe -d $Distro -- bash -lc $Command
    if ($LASTEXITCODE -ne 0) { throw "Build/test failed: $Command" }
}

$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.81-buf512-readonly-diag'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.82-s29gl256s-ace-buf512-runtime-programmer'
$zip="$bundle.zip"
$app=Join-Path $root 'build/program82-app/app.elf'
$readme=Join-Path $root 'app/gba_program82_ace_buf512/README_TEST82_CN.txt'
$rom=[IO.Path]::GetFullPath($RomPath)
$wrom='/mnt/'+$rom.Substring(0,1).ToLowerInvariant()+$rom.Substring(2).Replace('\','/')
$osRoot=if([IO.Path]::IsPathRooted($OsBuild)){$OsBuild}else{Join-Path $root $OsBuild}
$os=Join-Path $osRoot 'os.bin'
$osManifest=Join-Path $osRoot 'PROGRAM65_OS_SHA256.txt'
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'
$staPath=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$qsfPath=Join-Path $fpga 'ap_core.qsf'
$fpgaManifest=Join-Path $fpga 'PROGRAM65_SHA256.txt'
$utf8=[Text.UTF8Encoding]::new($false)
$expectedRomBytes=8705280L
$expectedRomMd5='07e851b1dc49009ebd00982514ebfe92'
$expectedRomSha='0d0945fff2edd4d4ae627ed003ebb0050937ee689d606ccff89c4ff2c2638571'
$expectedProgramCalls=17003
$expectedFullBlocks=17002
$expectedTailBytes=256

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke82 package'
}
foreach ($path in @($base,$readme,$rom,$os,$osManifest,$rawRbf,$staPath,$qsfPath,$fpgaManifest)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}
if ((Get-Item -LiteralPath $rom).Length -ne $expectedRomBytes -or
    (Get-FileHash -LiteralPath $rom -Algorithm MD5).Hash.ToLowerInvariant() -ne $expectedRomMd5 -or
    (Sha $rom) -ne $expectedRomSha) { throw 'Wrong test ROM' }
if ([math]::Floor($expectedRomBytes / 512) -ne $expectedFullBlocks -or
    $expectedRomBytes % 512 -ne $expectedTailBytes -or
    [math]::Ceiling($expectedRomBytes / 512) -ne $expectedProgramCalls) {
    throw 'Program block arithmetic is wrong'
}

foreach ($line in Get-Content -LiteralPath $osManifest) {
    if ($line -match '^EXTRA_CFLAGS=') {
        foreach ($flag in @('CART_MMIO_ENABLED=1','CART_S29GL256_PROGRAMMER=1',
                            'CHISBURNER_OUTPUT_ENABLED=1')) {
            if ($line -notmatch [regex]::Escape($flag)) { throw "OS missing $flag" }
        }
    } elseif ($line -match '^([0-9a-f]{64})  (.+)$') {
        if (!(Test-Path -LiteralPath $Matches[2]) -or (Sha $Matches[2]) -cne $Matches[1]) {
            throw "OS source drift: $($Matches[2])"
        }
    } else { throw "Malformed OS manifest: $line" }
}
foreach ($line in Get-Content -LiteralPath $fpgaManifest) {
    if ($line -notmatch '^([0-9a-f]{64})  (.+)$' -or
        !(Test-Path -LiteralPath $Matches[2]) -or (Sha $Matches[2]) -cne $Matches[1]) {
        throw "FPGA source drift: $line"
    }
}
$qsf=Get-Content -Raw -LiteralPath $qsfPath
if ($qsf -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$' -or
    $qsf -match '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_IDENT56\s*$') {
    throw 'FPGA build is not the isolated programmer profile'
}
$sta=Get-Content -Raw -LiteralPath $staPath
$slacks=@{}
foreach ($kind in @('setup','hold')) {
    $matches=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if (!$matches.Count) { throw "Missing $kind timing result" }
    $slack=($matches|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
    if ($slack -le 0) { throw "Refuse package: $kind timing failed $slack ns" }
    $slacks[$kind]=$slack
}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($os))

if (-not ('ChisBurner.Program82.Reverse' -as [type])) {
    Add-Type -TypeDefinition @'
namespace ChisBurner.Program82 {
    public static class Reverse {
        public static byte[] Apply(byte[] input) {
            var table = new byte[256];
            for (int i = 0; i < 256; ++i) {
                int x = i, y = 0;
                for (int bit = 0; bit < 8; ++bit) { y = (y << 1) | (x & 1); x >>= 1; }
                table[i] = (byte)y;
            }
            var output = new byte[input.Length];
            for (int i = 0; i < input.Length; ++i) output[i] = table[input[i]];
            return output;
        }
    }
}
'@
}
$raw=[IO.File]::ReadAllBytes($rawRbf)
$rbfR=[ChisBurner.Program82.Reverse]::Apply($raw)
if ([BitConverter]::ToString([ChisBurner.Program82.Reverse]::Apply([byte[]](1,128,18,170))) -ne '80-01-48-55') {
    throw 'RBF bit-reversal vector failed'
}

Wsl "cd '$wroot/app/gba_program82_ace_buf512' && make -B host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 ROM_PATH='$wrom'"
Wsl "cd '$wroot' && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gba_s29gl256 -Mdir build/program82-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v '$wroot/tests/test-program65-rtl.cpp' && build/program82-package-rtl/Vchisburner_cart_gba_s29gl256"
Wsl "cd '$wroot' && bash tests/run-program65-periph-rtl.sh"

$elf=[IO.File]::ReadAllBytes($app)
$elfText=[Text.Encoding]::ASCII.GetString($elf)
foreach ($requiredText in @('smoke.82','WAITING FOR PROGRAM ROM',
                             'LOAD PROGRAM ROM (SLOT 4)',
                             'FULL ERASE %u SECTORS',
                             '128 KiB SECTOR / %u B PROGRAM',
                             'A: ERASE+PROGRAM  B: CANCEL/RELOCK')) {
    if (!$elfText.Contains($requiredText)) { throw "Wrong smoke82 ELF: missing $requiredText" }
}
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243) { throw 'Wrong ELF format' }
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program82-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') { throw 'Unexpected app load map' }

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program82-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$maxWidth=($ui|ForEach-Object{$_.Length}|Measure-Object -Maximum).Maximum
if ($maxWidth -gt 40 -or ($ui|Where-Object{$_ -eq ' GBA PROGRAM smoke.82'}).Count -ne 11 -or
    !($ui -contains '128 KiB SECTOR / 512 B PROGRAM') -or
    !($ui -contains 'PROGRAM 17003/17003')) {
    throw "UI contract failed width=$maxWidth"
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if ($rel -in @('Assets/chisburner/common/chisburner81.elf',
                   'Assets/chisburner/common/chisburner81.ini')) { continue }
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}
$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $os -Destination (Join-Path $common 'os.bin') -Force
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),$rbfR)
$osHash=Sha (Join-Path $common 'os.bin')
$rbfHash=Sha (Join-Path $coreRoot 'chis.rbf_r')
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner82.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner82.ini'),
    "[os]`nELF=chisburner82.elf`nARGS=`n",$utf8)
Copy-Item -LiteralPath $readme -Destination (Join-Path $bundle 'README_TEST82_CN.txt')

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.82-s29gl256s-ace-buf512-runtime-programmer'
$core.core.metadata.date_release='2026-09-06'
$core.core.metadata.description='S29GL256S full 32 MiB erase and 512-byte buffered runtime ROM programmer'
if ([int64]$core.core.framework.hardware.cartridge_adapter -ne 16777216) {
    throw 'Play Cartridge option is not enabled'
}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    if ([int]$slot.id -eq 2) { $slot.filename='chisburner82.ini' }
    if ([int]$slot.id -eq 3) { $slot.filename='chisburner82.elf' }
}
$slot4=@($data.data.data_slots|Where-Object{[int]$_.id -eq 4})
if ($slot4.Count -ne 1 -or $slot4[0].required -or !$slot4[0].deferload -or
    [int]$slot4[0].parameters -ne 3 -or [int64]$slot4[0].size_exact -ne 8705280 -or
    $slot4[0].PSObject.Properties.Name -contains 'filename') {
    throw 'Runtime ROM slot policy drifted from smoke81'
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.82: destructive S29GL256S programmer; full detected 32 MiB erase as 256 x 128 KiB sectors; exact 8705280-byte runtime ROM; 17002 x 512-byte blocks plus 256-byte tail; final full-ROM MD5; two-stage physical confirmation.`n",$utf8)

foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $bundle -Recurse -File|Where-Object Extension -ieq '.gba') {
    throw 'ROM leaked into package'
}
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_REPORT.txt'),
    "smoke.82 package PASS`nmode=destructive S29GL256S 512-byte buffered programmer after explicit confirmation`nlaunch=Play Cartridge required`nslot4=optional user-reloadable core-specific deferload exact 8705280`nROM bytes=$expectedRomBytes`nROM MD5=$expectedRomMd5`nROM SHA256=$expectedRomSha`nfull erase=33554432 bytes, 256 x 131072-byte sectors`nprogram=$expectedRomBytes bytes, $expectedFullBlocks x 512 plus $expectedTailBytes-byte tail, $expectedProgramCalls calls`nverify=$expectedRomBytes-byte cartridge MD5 using 32-byte reads`nROM bundled=NO`nRTL=512-byte full block, 256-byte tail, 2-byte tail, boundary rejection, poll and recovery PASS`nsetup slack=$($slacks['setup']) ns`nhold slack=$($slacks['hold']) ns`nOS CRC32=$('{0:x8}' -f $osCrc.ComputedCrc)`nOS SHA256=$osHash`nRBF_R SHA256=$rbfHash`napp RV32 and ASAN/UBSAN program model=PASS`n",$utf8)
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$manifest=@();$prefix=[IO.Path]::GetFullPath($bundle).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $bundle -Recurse -File|Sort-Object FullName) {
    $rel=$file.FullName.Substring($prefix.Length).Replace('\','/')
    $manifest += "$(Sha $file.FullName)  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$manifest,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
"Package: $zip"
"SHA256: $(Sha $zip)"
"Destructive programmer: all gates and physical confirmation precede full 32 MiB erase"
