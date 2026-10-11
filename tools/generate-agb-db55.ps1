[CmdletBinding()]
param(
    [string]$InputJson = (Join-Path $PSScriptRoot '..\data\chisburner\db_AGB.json'),
    [string]$OutputC = (Join-Path $PSScriptRoot '..\app\gba_db55\db55_data.c')
)
$ErrorActionPreference = 'Stop'
$expectedHash = 'd52de244166c5b609c27bfe3aa5c195ab73c261ce2ec677cb18e593b1f596a85'
$expectedCommit = '0272d8ef039b2e131a7278c1a3b6dd4ade5a7ecd'
$actualHash = (Get-FileHash -LiteralPath $InputJson -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actualHash -ne $expectedHash) { throw "Unexpected db_AGB.json SHA256: $actualHash" }
$db = Get-Content -Raw -LiteralPath $InputJson | ConvertFrom-Json -AsHashtable
if ($db.Count -ne 2923) { throw "Expected 2923 entries, got $($db.Count)" }
$required = @('gn','ne','gc','rc','rs','st','ss','lg','rg')
$rows = [Collections.Generic.List[object]]::new()
foreach ($pair in $db.GetEnumerator()) {
    if ($pair.Key -cnotmatch '^[0-9a-f]{40}$') { throw "Invalid key $($pair.Key)" }
    foreach ($field in $required) {
        if (-not $pair.Value.ContainsKey($field)) { throw "$($pair.Key) missing $field" }
    }
    if ([string]$pair.Value.gc -notmatch '^.{4}$') { throw "$($pair.Key) invalid gc" }
    if ([uint64]$pair.Value.rs -notin @(1048576,2097152,4194304,8388608,16777216,33554432,67108864)) {
        throw "$($pair.Key) invalid rs"
    }
    if ([uint64]$pair.Value.st -gt 6) { throw "$($pair.Key) invalid st" }
    $rows.Add($pair)
}
$rows.Sort([Comparison[object]]{ param($a,$b) [string]::CompareOrdinal($a.Key,$b.Key) })
$sb = [Text.StringBuilder]::new(600000)
[void]$sb.AppendLine('/* Generated from the pinned third-party db_AGB.json. Do not hand edit. */')
[void]$sb.AppendLine('#include "db55.h"')
[void]$sb.AppendLine('const struct db55_entry db55_entries[] = {')
foreach ($pair in $rows) {
    $h = for ($i=0; $i -lt 40; $i+=2) { '0x' + $pair.Key.Substring($i,2) }
    $gc = [Text.Encoding]::UTF8.GetBytes([string]$pair.Value.gc)
    if ($gc.Count -ne 4) { throw "$($pair.Key) non-ASCII gc" }
    $flags = 0
    if ($pair.Value.ContainsKey('3d')) { $flags = $flags -bor 1; if ([bool]$pair.Value.'3d') { $flags = $flags -bor 2 } }
    if ($pair.Value.ContainsKey('rv')) { $flags = $flags -bor 4 }
    $line = '  {{' + ($h -join ',') + '},' + [uint64]$pair.Value.rs + 'u,' +
        [uint64]$pair.Value.rc + 'u,' + [uint64]$pair.Value.ss + 'u,{' +
        (($gc | ForEach-Object { '0x{0:x2}' -f $_ }) -join ',') + '},' +
        [uint64]$pair.Value.st + 'u,' + $flags + 'u,{0,0}},'
    [void]$sb.AppendLine($line)
}
[void]$sb.AppendLine('};')
[void]$sb.AppendLine('const uint32_t db55_entry_count = sizeof(db55_entries) / sizeof(db55_entries[0]);')
[void]$sb.AppendLine("const char db55_source_sha256[] = `"$expectedHash`";")
[void]$sb.AppendLine("const char db55_source_commit[] = `"$expectedCommit`";")
[IO.File]::WriteAllText([IO.Path]::GetFullPath($OutputC), $sb.ToString(), [Text.UTF8Encoding]::new($false))
Write-Output "PASS generated $($rows.Count) sorted entries -> $OutputC"
