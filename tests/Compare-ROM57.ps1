[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$DumpPath,
 [string]$SourcePath='C:/Users/lin13/Downloads/[1MSRAM][FTC]口袋妖怪 - 宝石_中文典藏版[漫游&TGB联合汉化].gba'
)
$ErrorActionPreference='Stop'
$expectedMd5='872A1A05E39006F3BEA69EFAA641B3A4'
$expectedSha='D1E41D5F8004DE024391BE96535BBC8952FB30A7B04B6E9C5A666D6927492DEC'
foreach($path in @($SourcePath,$DumpPath)){
 $file=Get-Item -LiteralPath $path
 if($file.PSIsContainer -or $file.Length -ne 33554432){throw "Wrong file size: $path (expected 33554432)"}
 $md5=(Get-FileHash -LiteralPath $file.FullName -Algorithm MD5).Hash
 $sha=(Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
 [pscustomobject]@{File=$file.FullName;Bytes=$file.Length;MD5=$md5;SHA256=$sha;Match=($md5 -eq $expectedMd5 -and $sha -eq $expectedSha)}
 if($md5 -ne $expectedMd5 -or $sha -ne $expectedSha){throw "Hash mismatch: $path"}
}
'PASS: source and dump have the exact expected size, MD5, and SHA256. Files were read only.'
