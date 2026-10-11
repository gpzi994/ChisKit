[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$RomPath='C:/Users/lin13/Desktop/game/逆转裁判.gba',
    [string]$OsBuild='.integration/openfpgaos/src/firmware/os/bld/program72-pocket-release',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-program65-s1-route-maxfit-smoke72-win'
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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.71-gls-profile-readonly'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.73-s29gl256s-ace-programmer-fix'
$zip="$bundle.zip"
$app=Join-Path $root 'build/program73-app/app.elf'
$readme=Join-Path $root 'app/gba_program73_ace/README_TEST73_CN.txt'
$osRoot=if([IO.Path]::IsPathRooted($OsBuild)){$OsBuild}else{Join-Path $root $OsBuild}
$os=Join-Path $osRoot 'os.bin'
$osManifest=Join-Path $osRoot 'PROGRAM65_OS_SHA256.txt'
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'
$staPath=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$qsfPath=Join-Path $fpga 'ap_core.qsf'
$fpgaManifest=Join-Path $fpga 'PROGRAM65_SHA256.txt'
$utf8=[Text.UTF8Encoding]::new($false)
$rom=[IO.Path]::GetFullPath($RomPath)
$expectedRomBytes=8705280L
$expectedRomMd5='07e851b1dc49009ebd00982514ebfe92'
$expectedRomSha='0d0945fff2edd4d4ae627ed003ebb0050937ee689d606ccff89c4ff2c2638571'
$baseZipHash='4145f012ce7494f7127ae8bd82213263f8f379b68db78c4ea318eb9a1ad193d2'

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke73 package'
}
foreach ($path in @($base,$readme,$rom,"$baseBundle.zip",$os,$osManifest,
                     $rawRbf,$staPath,$qsfPath,$fpgaManifest)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}
if ((Sha "$baseBundle.zip") -ne $baseZipHash) { throw 'Wrong smoke71 base package' }
if ((Get-Item -LiteralPath $rom).Length -ne $expectedRomBytes -or
    (Get-FileHash -LiteralPath $rom -Algorithm MD5).Hash.ToLowerInvariant() -ne $expectedRomMd5 -or
    (Sha $rom) -ne $expectedRomSha) {
    throw 'The selected Ace Attorney ROM does not match the tested source'
}

foreach ($line in Get-Content -LiteralPath $osManifest) {
    if ($line -match '^EXTRA_CFLAGS=') {
        foreach ($flag in @('CART_MMIO_ENABLED=1','CART_S29GL256_PROGRAMMER=1',
                            'CHISBURNER_OUTPUT_ENABLED=1')) {
            if ($line -notmatch [regex]::Escape($flag)) {
                throw "Programmer OS missing $flag"
            }
        }
    } elseif ($line -match '^([0-9a-f]{64})  (.+)$') {
        if (!(Test-Path -LiteralPath $Matches[2]) -or
            (Sha $Matches[2]) -cne $Matches[1]) {
            throw "Programmer OS source drift: $($Matches[2])"
        }
    } else {
        throw "Malformed programmer OS manifest: $line"
    }
}
foreach ($line in Get-Content -LiteralPath $fpgaManifest) {
    if ($line -notmatch '^([0-9a-f]{64})  (.+)$' -or
        !(Test-Path -LiteralPath $Matches[2]) -or
        (Sha $Matches[2]) -cne $Matches[1]) {
        throw "Programmer FPGA source drift: $line"
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

if (-not ('ChisBurner.Program73.Reverse' -as [type])) {
    Add-Type -TypeDefinition @'
namespace ChisBurner.Program73 {
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
$rbfR=[ChisBurner.Program73.Reverse]::Apply([IO.File]::ReadAllBytes($rawRbf))
$osHash=Sha $os

$wrom='/mnt/'+$rom.Substring(0,1).ToLowerInvariant()+$rom.Substring(2).Replace('\','/')
Wsl "cd '$wroot/app/gba_program65' && make host-test OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot/app/gba_program73_ace' && make -B host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 ROM_PATH='$wrom'"
Wsl "cd '$wroot' && rm -rf build/program73-package-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gba_s29gl256 -Mdir build/program73-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v '$wroot/tests/test-program65-rtl.cpp' && build/program73-package-rtl/Vchisburner_cart_gba_s29gl256"
Wsl "cd '$wroot' && bash tests/run-program65-periph-rtl.sh"

$elf=[IO.File]::ReadAllBytes($app)
$elfText=[Text.Encoding]::ASCII.GetString($elf)
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243 -or
    !$elfText.Contains('GBA PROGRAM %s') -or
    !$elfText.Contains('smoke.73') -or
    !$elfText.Contains('FULL ERASE %u SECTORS')) {
    throw 'Wrong smoke73 application ELF'
}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program73-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') { throw 'Unexpected smoke73 application load map' }

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program73-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach ($line in $ui) {
    if ($line -eq ' GBA PROGRAM smoke.73') {
        $maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages
    }
    ++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length)
    if ($line.Length -gt 40) { throw "UI line too wide: $line" }
}
$maxRows=[Math]::Max($maxRows,$rows)
foreach ($required in @('ERASE 256/256','PROGRAM 272040/272040','VERIFY 8705280/8705280')) {
    if (!($ui -contains $required)) { throw "Missing smoke73 UI contract: $required" }
}
if ($pages -ne 11 -or $maxRows -gt 18) {
    throw "UI shape failed pages=$pages rows=$maxRows width=$maxWidth"
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if ($rel -in @('Assets/chisburner/common/chisburner71.elf','Assets/chisburner/common/chisburner71.ini')) { continue }
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}
$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $os -Destination (Join-Path $common 'os.bin') -Force
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner73.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner73.ini'),"[os]`nELF=chisburner73.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),$rbfR)
Copy-Item -LiteralPath $readme -Destination (Join-Path $bundle 'README_TEST73_CN.txt')

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.73-s29gl256s-ace-programmer-fix'
$core.core.metadata.date_release='2026-09-06'
$core.core.metadata.description='S29GL256S programmer OS/RBF fix; full erase and exact 8705280-byte GBA program test'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    switch ([int]$slot.id) {
        2 { $slot.filename='chisburner73.ini' }
        3 { $slot.filename='chisburner73.elf' }
        4 {
            $slot.name='8705280 B GBA ROM TO PROGRAM'
            $slot.required=$true
            $slot.parameters=2
            $slot.extensions=@('gba')
            $slot.deferload=$true
            if ($slot.PSObject.Properties['size_exact']) {
                $slot.size_exact=$expectedRomBytes
            } else {
                $slot | Add-Member -NotePropertyName size_exact -NotePropertyValue $expectedRomBytes
            }
            foreach ($property in @('filename','nonvolatile','address','size_maximum','secondary')) {
                $slot.PSObject.Properties.Remove($property)
            }
        }
    }
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.73: programmer-enabled OS/RBF; exact S29GL256S only; full 32MiB sector erase; program and MD5-verify 8705280 bytes in conservative 32-byte write-buffer operations.`n",$utf8)

foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $bundle -Recurse -File | Where-Object {$_.Extension -ieq '.gba'}) {
    throw 'ROM must not be included in the package'
}
$rbfHash=Sha (Join-Path $coreRoot 'chis.rbf_r')
if ((Sha (Join-Path $common 'os.bin')) -ne $osHash) {
    throw 'Programmer OS changed while packaging'
}

[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_REPORT.txt'),
    "smoke.73 package PASS`nsmoke.72 result=TARGET IDENTIFY FAIL RC -4; no erase/write`nfix=programmer-enabled OS and RBF`nROM bytes=$expectedRomBytes`nROM MD5=$expectedRomMd5`nROM SHA256=$expectedRomSha`nfull erase=256 x 128 KiB`nsetup slack=$($slacks['setup']) ns`nhold slack=$($slacks['hold']) ns`nOS CRC32=$('{0:x8}' -f $osCrc.ComputedCrc)`nOS SHA256=$osHash`nRBF_R SHA256=$rbfHash`n",$utf8)
$manifest=@()
$prefix=[IO.Path]::GetFullPath($bundle).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $bundle -Recurse -File | Sort-Object FullName) {
    $rel=$file.FullName.Substring($prefix.Length).Replace('\','/')
    $manifest += "$(Sha $file.FullName)  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$manifest,$utf8)

Compress-Archive -LiteralPath $bundle -DestinationPath $zip -CompressionLevel Optimal
$zipSha=Sha $zip
"Package: $zip"
"SHA256: $zipSha"
