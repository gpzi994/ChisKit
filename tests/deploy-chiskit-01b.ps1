$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$pkg="$root/build/releases/chiskit-0.1b"
if(!(Test-Path 'D:/Cores/ChisBurner.ChisBurner/core.json')){throw 'Expected Pocket not mounted'}
$plan=@()
foreach($line in Get-Content "$pkg/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 $src="$pkg/SD_ROOT/$p";$dst="D:/$p"
 if((Get-FileHash -LiteralPath $src).Hash-ne$hash){throw "Package mismatch $p"}
 $plan+=@{Source=$src;Target=$dst;Hash=$hash}
}
$old='D:/Saves/chisburner/common'
foreach($f in Get-ChildItem -LiteralPath $old -Recurse -File){
 $rel=$f.FullName.Substring($old.Length+1).Replace('\','/')
 if($rel-eq'DIRECTORY.txt'){continue}
 if($rel-eq'chisburner-language.cfg'){$rel='chiskit-language.cfg'}
 $plan+=@{Source=$f.FullName;Target="D:/Saves/chiskit/common/$rel";Hash=(Get-FileHash -LiteralPath $f.FullName).Hash}
}
# Preflight ALL conflicts before writing anything. Never replace user data.
foreach($p in $plan){if((Test-Path -LiteralPath $p.Target)-and(Get-FileHash -LiteralPath $p.Target).Hash-ne$p.Hash){throw "Conflicting existing file: $($p.Target)"}}
foreach($p in $plan){
 if(!(Test-Path -LiteralPath $p.Target)){
  New-Item -ItemType Directory -Path (Split-Path $p.Target) -Force|Out-Null
  Copy-Item -LiteralPath $p.Source -Destination $p.Target
 }
 if((Get-FileHash -LiteralPath $p.Target).Hash-ne$p.Hash){throw "Readback mismatch $($p.Target)"}
}
Write-Host "chiskit 0.1b PASS: $($plan.Count) files installed/migrated and SHA256 verified. Old core and all source files preserved."
