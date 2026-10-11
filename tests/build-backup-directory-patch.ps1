$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.102-backup-directory-patch'
if((Test-Path -LiteralPath $bundle)-or(Test-Path -LiteralPath "$bundle.zip")){
 throw 'Preserve existing directory patch'
}
$output=Join-Path $bundle 'SD_ROOT/Saves/chisburner/common'
New-Item -ItemType Directory -Path $output | Out-Null
# A file keeps the otherwise empty directory present when the ZIP is extracted.
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText((Join-Path $output 'DIRECTORY.txt'),"ChisBurner backup output directory. This file is not a ROM or save file.`n",$utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'README.txt'),@'
ChisBurner smoke.102 - directory-only diagnostic patch

Copy the Saves folder inside SD_ROOT to the ROOT of the Pocket SD card.
The resulting directory must be /Saves/chisburner/common/.
Do not place SD_ROOT itself on the card.

This patch contains no core, OS, application, ROM, or save replacement.
Do not delete or overwrite existing backups. DIRECTORY.txt is only a marker.
Keep smoke.102 installed. Restart its core, then run Backup ROM once.
Record the phase, result code, and whether a new cb102-*.gba exists in
Saves/chisburner/common. A created file alone does not establish dump validity;
the transfer and full readback verification must also succeed.

This isolates missing parent directories. It is NOT a command-handshake fix,
NOT an MBC3 fix, and NOT a claim that backup is now verified on hardware.
'@,$utf8)
$files=@(Get-ChildItem (Join-Path $bundle 'SD_ROOT') -Recurse -File)
if($files.Count -ne 1 -or $files[0].Name -ne 'DIRECTORY.txt'){
 throw 'Directory patch must not include firmware or user data'
}
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath "$bundle.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead("$bundle.zip")
try{
 $entries=@($zip.Entries | Where-Object { $_.FullName.Replace('\','/') -eq 'SD_ROOT/Saves/chisburner/common/DIRECTORY.txt' })
 if($entries.Count -ne 1){throw 'ZIP is missing the required directory marker'}
}finally{$zip.Dispose()}
Get-FileHash -LiteralPath "$bundle.zip" -Algorithm SHA256
