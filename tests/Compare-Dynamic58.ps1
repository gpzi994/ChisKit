[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)]
    [string]$TfRoot
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($TfRoot)
$file=[IO.Path]::GetFullPath((Join-Path $root 'Saves\chisburner\common\dyn58-a7c9.bin'))
if(!$file.StartsWith($root.TrimEnd('\')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'Resolved test file escaped the supplied TF root'
}
if(!(Test-Path -LiteralPath $file)){throw "Dynamic output file not found: $file"}
$length=(Get-Item -LiteralPath $file).Length
$sha=(Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant()
$md5=(Get-FileHash -LiteralPath $file -Algorithm MD5).Hash.ToLowerInvariant()
$ok=$length -eq 8192 -and $sha -eq 'b7708bc5b3bb230ceda2d59eb2250a6044203230c4bd14fc35068c59fc7c43c1'
[pscustomobject]@{
    Path=$file
    Length=$length
    SHA256=$sha
    MD5=$md5
    ExpectedLength=8192
    ExpectedSHA256='b7708bc5b3bb230ceda2d59eb2250a6044203230c4bd14fc35068c59fc7c43c1'
    Result=if($ok){'PASS'}else{'FAIL'}
}
if(!$ok){exit 1}
