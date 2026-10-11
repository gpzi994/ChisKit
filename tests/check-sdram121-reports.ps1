param([string]$reportRoot = (Join-Path $PSScriptRoot '../build/sdram121/quartus'))
$ErrorActionPreference = 'Stop'
$corners = @(Get-ChildItem -LiteralPath $reportRoot -Filter 'cdc-net-delay-?.txt')
if ($corners.Count -ne 4) { throw 'Expected four operating-corner CDC reports' }
foreach ($file in $corners) {
    $rows = @(Select-String -LiteralPath $file.FullName -Pattern '^; set_net_delay')
    $paths = @(Select-String -LiteralPath $file.FullName -Pattern '^;  --')
    if ($rows.Count -ne 5 -or $paths.Count -ne 66) {
        throw "CDC coverage missing: $($file.Name), groups=$($rows.Count), paths=$($paths.Count)"
    }
    foreach ($row in $rows) {
        $slack = [double]::Parse(($row.Line -split ';')[2].Trim(), [cultureinfo]::InvariantCulture)
        if ($slack -lt 0) { throw "CDC delay violation: $($row.Line)" }
    }
}
$summary = Get-Content -LiteralPath (Join-Path $reportRoot 'output_files/ap_core.sta.summary') -Raw
foreach ($kind in @('Setup','Hold','Recovery','Removal','Minimum Pulse Width')) {
    $rows = [regex]::Matches($summary, 'Type\s+: [^\r\n]*'+$kind+'[^\r\n]*\r?\nSlack : ([\d.-]+)')
    if (!$rows.Count) { throw "Missing $kind reports" }
    foreach ($row in $rows) {
        if ([double]::Parse($row.Groups[1].Value,[cultureinfo]::InvariantCulture) -lt 0) {
            throw "$kind timing violation: $($row.Value)"
        }
    }
}
$sync = @(Select-String -LiteralPath (Join-Path $reportRoot 'cdc-metastability.txt') -Pattern '^; .*sd120.*; .*; .*; Yes')
if ($sync.Count -ne 45) { throw "Expected 45 new CDC chains; found $($sync.Count)" }
Write-Host 'PASS SDRAM121: 4 corners, 5 CDC groups/66 paths per corner, 45 new synchronizer chains; constrained timing nonnegative.'
Write-Host 'NOT full I/O signoff or hardware validation. Review unconstrained.txt before any release.'
