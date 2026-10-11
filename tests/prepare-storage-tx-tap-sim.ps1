# Test-only Verilator compatibility lowering of procedural inout-reg drivers.
# Production RTL is NEVER edited here. State machines/taps are unchanged;
# pad assignments are routed through one continuous tristate driver per pin.
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src=Join-Path $root '.integration\openfpgaos\src\fpga\targets\pocket\apf\io_bridge_peripheral.v'
$outDir=Join-Path $root 'build\rtl\storage-tx-tap'
New-Item -ItemType Directory -Path $outDir -Force | Out-Null
$code=Get-Content -Raw -LiteralPath $src
$drivers=''
foreach($pin in @('phy_spiclk','phy_spimosi','phy_spimiso')) {
    if([regex]::Matches($code,"inout\s+reg\s+$pin\b").Count -ne 1) { throw "Unexpected pin declaration: $pin" }
    $code=[regex]::Replace($code,"inout\s+reg\s+$pin\b","inout wire $pin")
    $code=[regex]::Replace($code,"\b$pin\s*<=\s*([^;]+);",[Text.RegularExpressions.MatchEvaluator]{
        param($match)
        $value=$match.Groups[1].Value.Trim()
        if($value -eq "1'bZ") { "begin sim_drive_$pin <= 0; sim_oe_$pin <= 0; end" }
        else { "begin sim_drive_$pin <= $value; sim_oe_$pin <= 1; end" }
    })
    $drivers += "reg sim_drive_$pin, sim_oe_$pin;`nassign $pin = sim_oe_$pin ? sim_drive_$pin : 1'bz;`n"
}
$code=$code.Replace('endmodule',($drivers+"`nendmodule"))
[IO.File]::WriteAllText((Join-Path $outDir 'io_bridge_peripheral_sim.v'),$code,(New-Object Text.UTF8Encoding($false)))
$sha=[Security.Cryptography.SHA256]::Create()
try { $hash=[BitConverter]::ToString($sha.ComputeHash([IO.File]::ReadAllBytes($src))).Replace('-','').ToLowerInvariant() }
finally { $sha.Dispose() }
[IO.File]::WriteAllText((Join-Path $outDir 'ADAPTER.txt'),"Test-only tristate-driver lowering; NOT the compiled FPGA source. FSM and tap logic are unchanged.`nSource SHA256: $hash`n")
