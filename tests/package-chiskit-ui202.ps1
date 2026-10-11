$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/releases/casefix/ChisKit-0.1b"
$dst="$root/build/releases/ui202/ChisKit-0.1b"
if(Test-Path $dst){throw 'Preserve existing package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $h,$p=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$p").Hash-ne$h){throw "Base mismatch $p"}
}
New-Item -ItemType Directory -Path $dst -Force|Out-Null
Copy-Item "$base/SD_ROOT" "$dst/SD_ROOT" -Recurse
Copy-Item "$root/build/gb-rtc202-app/app.elf" "$dst/SD_ROOT/Assets/chiskit/common/chiskit.elf"
foreach($relative in @('Cores/Alius.chiskit/core.json','Platforms/chiskit.json')){
 $p="$dst/SD_ROOT/$relative";$j=Get-Content $p -Raw|ConvertFrom-Json
 if($j.core){$j.core.metadata.shortname='ChisKit';$j.core.cores[0].name='ChisKit'}else{$j.platform.name='ChisKit'}
 [IO.File]::WriteAllText($p,($j|ConvertTo-Json -Depth 30),[Text.UTF8Encoding]::new($false))
}
foreach($relative in @('Cores/Alius.chiskit/info.txt','Saves/chiskit/common/DIRECTORY.txt')){
 $p="$dst/SD_ROOT/$relative"
 [IO.File]::WriteAllText($p,([IO.File]::ReadAllText($p).Replace('chiskit','ChisKit')),[Text.UTF8Encoding]::new($false))
}
Copy-Item "$base/README_CN.md" "$dst/README_CN.md"
Add-Content "$dst/README_CN.md" "`nDisplay name corrected to ChisKit. Version remains 0.1b; lowercase platform IDs and storage paths intentionally unchanged."
$changed=@('Assets/chiskit/common/chiskit.elf','Cores/Alius.chiskit/core.json','Platforms/chiskit.json','Cores/Alius.chiskit/info.txt','Saves/chiskit/common/DIRECTORY.txt')
$lines=Get-ChildItem "$dst/SD_ROOT" -File -Recurse|Sort-Object FullName|ForEach-Object{
 $p=$_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/');$h=(Get-FileHash $_.FullName).Hash
 if($p-notin$changed-and$h-ne(Get-FileHash "$base/SD_ROOT/$p").Hash){throw "Unexpected change $p"}
 "$h  $p"
}
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$lines)
Compress-Archive "$dst/*" "$dst.zip"
# Verify current installation before replacing only five known release files.
foreach($p in $changed){if((Get-FileHash "D:/$p").Hash-ne(Get-FileHash "$base/SD_ROOT/$p").Hash){throw "USB differs: $p"}}
$backup="$root/build/usb-before-chiskit-case-$(Get-Date -Format yyyyMMdd-HHmmss)"
foreach($p in $changed){
 New-Item -ItemType Directory -Path (Split-Path "$backup/$p") -Force|Out-Null
 Copy-Item "D:/$p" "$backup/$p"
 if((Get-FileHash "$backup/$p").Hash-ne(Get-FileHash "D:/$p").Hash){throw 'Backup mismatch'}
}
foreach($p in $changed){
 Copy-Item "$dst/SD_ROOT/$p" "D:/$p" -Force
 if((Get-FileHash "D:/$p").Hash-ne(Get-FileHash "$dst/SD_ROOT/$p").Hash){throw 'USB readback mismatch'}
}
Write-Host "PASS ChisKit name correction; five files deployed; settings and saves untouched; rollback $backup"
