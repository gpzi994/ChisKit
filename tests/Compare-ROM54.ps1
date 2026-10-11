[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$DumpPath,
      [string]$SourcePath='C:/Users/lin13/Desktop/game/逆转裁判.gba')
$ErrorActionPreference='Stop'
$expected='07E851B1DC49009EBD00982514EBFE92'
foreach($path in @($SourcePath,$DumpPath)){
 $file=Get-Item -LiteralPath $path
 if($file.PSIsContainer -or $file.Length -ne 8705280){throw "Wrong file size: $path (expected 8705280)"}
 $hash=(Get-FileHash -LiteralPath $file.FullName -Algorithm MD5).Hash
 [pscustomobject]@{File=$file.FullName;Bytes=$file.Length;MD5=$hash;Match=($hash -eq $expected)}
 if($hash -ne $expected){throw "MD5 mismatch: $path"}
}
'PASS: both files have the exact expected length and MD5. Files were read only.'
