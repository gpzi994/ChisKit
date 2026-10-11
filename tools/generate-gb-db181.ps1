$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$inputFile="$root/data/chisburner/db_DMG.json"
$expected='8A6E7517DE1D9B1AF04CC38838F2CFA75091D68907BF9B8144E27819CBBED153'
if((Get-FileHash -LiteralPath $inputFile).Hash-ne$expected){throw 'DMG database hash changed'}
$db=Get-Content -LiteralPath $inputFile -Raw|ConvertFrom-Json -AsHashtable
if($db.Count-ne3130){throw 'Unexpected DMG database count'}
$sb=[Text.StringBuilder]::new()
[void]$sb.AppendLine('/* Generated identity-only index, FlashGBX 0272d8ef039b2e131a7278c1a3b6dd4ade5a7ecd. */')
[void]$sb.AppendLine("/* db_DMG.json SHA256 $expected; no save-capacity fields exist. */")
[void]$sb.AppendLine('static const uint8_t gb181_hashes[][20]={')
foreach($key in ($db.Keys|Sort-Object)){
 if($key-cnotmatch '^[0-9a-f]{40}$'){throw 'Invalid hash key'}
 $bytes=for($i=0;$i-lt40;$i+=2){'0x'+$key.Substring($i,2)}
 [void]$sb.AppendLine(' {'+($bytes-join',')+'},')
}
[void]$sb.AppendLine('};')
[IO.File]::WriteAllText("$root/app/ui98/gb_db181_data.inc",$sb.ToString(),[Text.UTF8Encoding]::new($false))
'Generated 3130 GB identity records; no invented save sizes.'
