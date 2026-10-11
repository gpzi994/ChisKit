$ErrorActionPreference='Stop'
$source=(Resolve-Path -LiteralPath 'D:/Saves/chisburner/common').Path
if($source -ne 'D:\Saves\chisburner\common'){throw 'Unexpected source'}
$archive='C:\ONS_FPGA\work\ChisBurner\build\pocket-history-20260925'
if(Test-Path -LiteralPath $archive){throw 'Archive already exists'}
$protected=@('dump57-32m.gba','sram_lvbaoshi.gba','FLASH_LVBAOSHI.gba','SAV_LVBAOSHI_JIBA.sav','dump54.gba','dump53.gba','nizhuan.gba')
$before=@{}
foreach($name in $protected){$before[$name]=(Get-FileHash -LiteralPath (Join-Path $source $name)).Hash}
$files=@(Get-ChildItem -LiteralPath $source -File | Where-Object {
 $_.Name -notin $protected -and (
 $_.Name -match '^cb(102|106|107)-([a-z]+-)?[0-9a-f]{8}\.(gba|gbc)$' -or
 $_.Name -match '^ee139-[0-9a-f]{8}-(large|small)\.bin$' -or
 $_.Name -match '^(hd15[012]|st141|sv14[89])-[0-9a-f]{8}-[0-9a-f]{4}\.txt$' -or
 $_.Name -match '^sram13[78]-[0-9a-f]{8}\.sav$' -or
 $_.Name -match '^GBA-POKEMON_EMER-BPEE___0-[0-9a-f]{8}\.orig\.sav$')
})
$null=New-Item -ItemType Directory -Path $archive
$manifest=@(foreach($file in $files){
 if($file.DirectoryName -ne $source -or ($file.Attributes -band [IO.FileAttributes]::ReparsePoint)){throw 'Unsafe source'}
 $dest=Join-Path $archive $file.Name
 if([IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($dest)) -ne $archive){throw 'Unsafe destination'}
 $hash=(Get-FileHash -LiteralPath $file.FullName).Hash
 Copy-Item -LiteralPath $file.FullName -Destination $dest
 if((Get-FileHash -LiteralPath $dest).Hash -ne $hash){throw 'Copy verification failed'}
 [pscustomobject]@{Name=$file.Name;Bytes=$file.Length;SHA256=$hash}
})
$manifest | Export-Csv -LiteralPath (Join-Path $archive 'manifest.csv') -NoTypeInformation -Encoding utf8
# Only remove individually verified files, never a directory or recursive target.
foreach($row in $manifest){
 $path=Join-Path $source $row.Name
 if([IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($path)) -ne $source -or $row.Name -in $protected){throw 'Unsafe removal target'}
 if((Get-FileHash -LiteralPath $path).Hash -ne $row.SHA256){throw 'Source changed'}
 if((Get-FileHash -LiteralPath (Join-Path $archive $row.Name)).Hash -ne $row.SHA256){throw 'Archive changed'}
 Remove-Item -LiteralPath $path
}
foreach($name in $protected){if((Get-FileHash -LiteralPath (Join-Path $source $name)).Hash -ne $before[$name]){throw 'Protected file changed'}}
Write-Output "Archived $($manifest.Count) files; $([math]::Round(($manifest|Measure-Object Bytes -Sum).Sum/1MB,2)) MiB; protected seven files verified unchanged."
Get-ChildItem -LiteralPath $source -File | Select-Object Name,Length
