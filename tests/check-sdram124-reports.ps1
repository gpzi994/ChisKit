param([string]$reportRoot = (Join-Path $PSScriptRoot '../build/sdram124/quartus'),
      [switch]$AllowUnusedBridgeReady)
$ErrorActionPreference = 'Stop'
$corners = @(Get-ChildItem -LiteralPath $reportRoot -Filter 'cdc-net-delay-?.txt')
if ($corners.Count -ne 4) { throw 'Expected four operating-corner CDC reports' }
foreach ($file in $corners) {
    $rows = @(Select-String -LiteralPath $file.FullName -Pattern '^; set_net_delay')
    $paths = @(Select-String -LiteralPath $file.FullName -Pattern '^;  --')
    # The real APF bridge ties byte-address bits 1:0 low (map report).
    # Recorder bits 32:33 are therefore constant, leaving 68 CDC data bits.
    $expectedPaths=if($AllowUnusedBridgeReady){$paths.Count}else{134}
    if ($rows.Count -ne 6 -or
        ($AllowUnusedBridgeReady -and $paths.Count -notin @(133,134)) -or
        (!$AllowUnusedBridgeReady -and $paths.Count -ne 134)) {
        throw "CDC coverage missing: $($file.Name), groups=$($rows.Count), paths=$($paths.Count)"
    }
    if($AllowUnusedBridgeReady -and $paths.Count -eq 133){
        $report=Get-Content -LiteralPath $file.FullName -Raw
        if($report -notmatch 'publish_valid\}\]\s*; \[get_registers \{\*\|sd120\|reader\|transport\|bridge_ready\}\]\s*; max' -or
           $report -notmatch 'set_net_delay ; No nets found ; 8\.000\s*; No nets found\s*; \[get_registers \{\*\|sd120\|reader\|transport\|publish_valid\}\]'){
            throw "Missing path is not the unused bridge_ready: $($file.Name)"
        }
        $map=Get-Content -LiteralPath (Join-Path $reportRoot 'output_files/ap_core.map.rpt') -Raw
        if($map -notmatch 'bridge_ready ; Output ; Info\s*; Connected to dangling logic'){
            throw 'bridge_ready is not proven dangling by map report'
        }
    }
    $indices=@(foreach($p in $paths){if($p.Line -match 'first_c\[(\d+)\]') {[int]$Matches[1]}}) | Sort-Object -Unique
    if(($indices -join ',') -cne ((@(0..5)+@(34..95)) -join ',')){throw 'Recorder CDC bit coverage mismatch'}
    foreach ($row in $rows) {
        if($AllowUnusedBridgeReady -and $row.Line -match 'bridge_ready' -and
           $row.Line -match 'No nets found'){continue}
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
if ($sync.Count -ne 46) { throw "Expected 46 new CDC chains; found $($sync.Count)" }
Write-Host "PASS SDRAM124: 4 corners, 6 CDC groups/$expectedPaths paths per corner, 46 new synchronizer chains; constrained timing nonnegative."
Write-Host 'NOT full I/O signoff or hardware validation. Review unconstrained.txt before any release.'
