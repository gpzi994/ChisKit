$ErrorActionPreference='Stop'
. "$PSScriptRoot/chiskit-package-layout.ps1"
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/releases/verify204/ChisKit-0.1b"
$dst="$root/build/releases/ui205/ChisKit-0.1b"
$rel='Assets/chiskit/common/chiskit.elf'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing release'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Get-FileHash -LiteralPath (Resolve-ChisKitBaselineFile "$base/SD_ROOT" $p)).Hash-ne$hash){throw "Base mismatch $p"}
}
if((Get-FileHash "D:/$rel").Hash-ne(Get-FileHash "$base/SD_ROOT/$rel").Hash){throw 'Unexpected installed ELF'}
New-Item -ItemType Directory -Path $dst -Force|Out-Null
Copy-Item "$base/SD_ROOT" "$dst/SD_ROOT" -Recurse
Initialize-ChisKitCoreDirectory "$dst/SD_ROOT"
Copy-Item "$root/build/gb-rtc205-app/app.elf" "$dst/SD_ROOT/$rel"
Copy-Item "$base/README_CN.md" "$dst/README_CN.md"
Add-Content "$dst/README_CN.md" "`nUI205: reset and gear redrawn as native 14x14 square-pixel masks; removed vertical-only scaling. OS/FPGA and GB204 verification unchanged. UI host tests and native-size preview checked."
$lines=Get-ChildItem "$dst/SD_ROOT" -File -Recurse|Sort-Object FullName|ForEach-Object{
 $p=$_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/');$h=(Get-FileHash $_.FullName).Hash
 if($p-ne$rel-and$h-ne(Get-FileHash (Resolve-ChisKitBaselineFile "$base/SD_ROOT" $p)).Hash){throw "Unexpected change $p"}
 "$h  $p"
}
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$lines)
Assert-ChisKitPackageLayout "$dst/SD_ROOT"
Compress-Archive "$dst/*" "$dst.zip"
$backup="$root/build/usb-before-ui205-$(Get-Date -Format yyyyMMdd-HHmmss)"
New-Item -ItemType Directory -Path $backup|Out-Null
Copy-Item "D:/$rel" "$backup/chiskit.elf"
if((Get-FileHash "$backup/chiskit.elf").Hash-ne(Get-FileHash "D:/$rel").Hash){throw 'Backup mismatch'}
Copy-Item "$dst/SD_ROOT/$rel" "D:/$rel" -Force
if((Get-FileHash "D:/$rel").Hash-ne(Get-FileHash "$dst/SD_ROOT/$rel").Hash){throw 'USB readback mismatch'}
foreach($p in @('Assets/chiskit/common/os.bin','Cores/Alius.ChisKit/chiskit.rbf_r')){
 if((Get-FileHash "D:/$p").Hash-ne(Get-FileHash (Resolve-ChisKitBaselineFile "$base/SD_ROOT" $p)).Hash){throw "OS/FPGA mismatch $p"}
}
Write-Host "PASS UI205 application-only deployment; GB204 OS/FPGA unchanged; rollback $backup"
