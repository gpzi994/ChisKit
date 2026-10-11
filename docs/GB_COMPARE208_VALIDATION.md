# GB208 integrated comparison validation

## Scope and tools

ChisKit 0.1b, internal experiment 208. Uses sim-bringup, cdc-audit and
timing-closure methodology with Icarus/host GCC/Quartus Prime Lite 25.1std,
not the skills' Vivado/SynthPilot backend. No speed or hardware compatibility
claim until cartridge testing. No GBA comparator source change:
SHA256 `0F14BE81C8A8EF2DC00A6F619F47C0ABDA8F1D82BAF4DED75BB560D7A130AC22`.

## Changes

- Independent GB207 byte comparator selected by descriptor flags 0x80.
  Existing GBA descriptor 0x81 selects the original engine.
- Register index15 read-only capability GB20 (0x47423230); descriptor write
  remains reserved-zero. Unsupported older FPGA falls back without revoking token.
- OS selects MBC3/30/5 bank before each aligned <=16 KiB job, validates token,
  programmed/erased extent and accessible capacity. Existing submitter retains
  ownership through drain, times out and cancels. Cleanup restores bank1.
- Application routes GB final whole-NOR verification to hardware. Original
  software fallback retained. Image tail compares FF without PSRAM payload fetch.
- GBA algorithm, NOR programming sizes, save/RTC logic, themes and names unchanged.

## Functional evidence

- Standalone: 41 cases / 197117 assertions, bank boundaries, byte lanes, all tail
  positions, fault locations, response faults, abort in every active state.
- Integrated real NOR timing/PSRAM adapter: 13 cases each at S/M half-periods
  5/7 and 7/5; full banks0/1/256/511, partial FF tail, no PSRAM tail reads,
  mismatch byte/address, pending-memory and active-NOR cancel/drain/restart,
  GBA-to-GB mode transition. No cartridge writes during compare asserted.
  Mapper hardware is modeled; physical bank switching requires hardware test.
- Host OS: production code with MMIO model, old-FPGA fallback preserves token,
  malformed ranges rejected, byte descriptor, completion/cancel, mapper final
  banks127/255/511, restore bank1 and invalidate cache, no out-of-range bus access.
- Existing stream/program/GBA compare suite passed. GB save/restore production
  wrapper, GB RTC191 clock and RTC189, GBA save171, EEPROM162, RTC172 regressions
  passed. Logs: `build/gb-compare208-tests/`.

## CDC delta review

New comparator, descriptor mux, raw NOR adapter and register interface are all
in clk_cpu (100 MHz). No new asynchronous interface or reset domain added.
PSRAM uses unchanged program_path131/memory_path, crossing clk_cpu -> clk_74a
(74.25 MHz). Request/ack use three-stage synchronizers; address/data are held
through the four-phase handshake and captured only after synchronized control.
Cancellation suppresses delivery but drains the physical read before releasing
the lease. Source and memory reset release remains the existing top-level logic.
Lease/arbiter regression covers three clock ratios and lifecycle/cancel drain.

This is a change-focused structural review and behavioral evidence, NOT a full
Vivado report_cdc signoff. Existing async clock groups and constraints unchanged;
no new false paths, waivers, or relaxed periods. Existing external cartridge
I/O timing limitations remain. Quartus metastability/CDC-delay reports are
collected separately from functional simulation and constrained setup/hold.

## Implementation

Full clean synthesis/fit: `build/gb-compare208-quartus`, seed3, device5CEBA4F23C8.
Initial failure was a Windows-backslash Tcl source path; corrected to forward
slashes. Successful full compile elapsed 11m03s, 0 errors, 988 warnings.
After the OS capability readback guard was finalized, firmware MIF is updated
and the same fitted design reassembled; RTL/routing is unchanged by that update.

Fit resources compared with the inherited 191/204 implementation:

| Item | Baseline | GB208 |
| --- | ---: | ---: |
| ALMs | 13104 | 13170 / 18480 (71%) |
| Registers | 19667 | 20040 |
| Block memory bits | 1690583 | 1690583 |
| RAM blocks | 210 | 210 |
| DSP blocks | 5 | 5 |

Net fit increase 66 ALMs, not the standalone engine cost (packing/optimization
also changes). No additional block RAM. Existing warning categories remain,
including loop-variable/later-optimized legacy latch warnings and unmatched
legacy stride filter. Do not present this as a warning-free project.

Final post-fit STA: all four operating corners, minimum setup +0.534 ns,
hold +0.116 ns, recovery +2.801 ns, removal +0.257 ns, pulse width +0.555 ns.
All reported TNS zero. Existing SDRAM CDC checker passes 6 groups/134 paths
per corner and 46 synchronizer chains. PSRAM req/ack three-stage chains are
present in the fresh metastability report; their MTBF is Not Calculated, so
no quantitative MTBF claim is made. No unconstrained clocks; inherited 52
input and91 output ports remain unconstrained. NOT full external I/O signoff.

Package script `tests/package-chiskit208.ps1` verifies the timer206 baseline,
OS CRC/ABI integrity and fitted firmware MIF hash; changes only app/OS/RBF_R.
Optional USB deployment checks the installed baseline, backs up all three
files and verifies every copied file by SHA256. Saves/settings untouched.
