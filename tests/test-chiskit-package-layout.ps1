$ErrorActionPreference='Stop'
. "$PSScriptRoot/chiskit-package-layout.ps1"
$temp=Join-Path ([IO.Path]::GetTempPath()) ('chiskit-layout-'+[guid]::NewGuid().ToString('N'))
$legacy=Join-Path $temp 'Cores/Alius.chiskit'
New-Item -ItemType Directory -Path $legacy -Force | Out-Null
[IO.File]::WriteAllText((Join-Path $legacy 'core.json'),'{"core":{"metadata":{"author":"Alius","shortname":"ChisKit"}}}')
$before=(Get-FileHash (Join-Path $legacy 'core.json')).Hash
function Expect-Rejection([scriptblock]$Action){
 $rejected=$false
 try { & $Action } catch { $rejected=$true }
 if(!$rejected){throw 'Expected rejection'}
}
Expect-Rejection { Assert-ChisKitPackageLayout $temp }
if(!(Test-Path (Resolve-ChisKitBaselineFile $temp 'Cores/Alius.ChisKit/core.json'))){throw 'Legacy lookup failed'}
Initialize-ChisKitCoreDirectory $temp
Initialize-ChisKitCoreDirectory $temp
Assert-ChisKitPackageLayout $temp
$core=Join-Path $temp 'Cores/Alius.ChisKit/core.json'
if((Get-FileHash $core).Hash -ne $before){throw 'Normalization modified payload'}
Compress-Archive -Path "$temp/Cores" -DestinationPath "$temp/layout.zip"
$zip=[IO.Compression.ZipFile]::OpenRead("$temp/layout.zip")
try {
 if(@($zip.Entries | Where-Object { $_.FullName.Replace('\','/') -ceq 'Cores/Alius.ChisKit/core.json' }).Count -ne 1){throw 'ZIP casing wrong'}
}finally{$zip.Dispose()}
[IO.File]::WriteAllText($core,'{"core":{"metadata":{"author":"Alius","shortname":"chiskit"}}}')
Expect-Rejection { Assert-ChisKitPackageLayout $temp }
foreach($file in Get-ChildItem $PSScriptRoot -File -Filter '*.ps1'){
 $tokens=$null;$errors=$null
 $null=[Management.Automation.Language.Parser]::ParseFile($file.FullName,[ref]$tokens,[ref]$errors)
 if($errors){throw "$($file.Name): $errors"}
}
Write-Host "PASS legacy normalization, idempotence, payload hashes, ZIP casing, metadata rejection and PowerShell parsing. Fixtures: $temp"
