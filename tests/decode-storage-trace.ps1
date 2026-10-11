[CmdletBinding()]
param([Parameter(Mandatory)][string]$Trace)
$hex = $Trace.Trim() -replace '^0[xX]', ''
[uint32]$word = [Convert]::ToUInt32($hex, 16)
if (($word -shr 28) -ne 13) { throw 'Not a smoke.44 trace word (must start with D)' }
$bridge = ($word -shr 12) -band 0xFFFF
$prefix = @('other', 'cm: request', 'bu: busy', 'ok: complete', 'reserved', 'reserved', 'reserved', 'reserved')[($bridge -shr 9) -band 7]
$flags = $bridge -band 255
[pscustomobject]@{
    Trace = '{0:X8}' -f $word
    TargetState = ($bridge -shr 12) -band 15
    TargetPrefix = $prefix
    TargetLowWordIs0188 = [bool]($bridge -band 256)
    BridgeSawFlushRequest = [bool]($flags -band 128)
    BridgeLaunchedFlush = [bool]($flags -band 64)
    HostReadCommand = [bool]($flags -band 32)
    HostReadSlotParameter = [bool]($flags -band 16)
    HostWroteStatus = [bool]($flags -band 8)
    HostWroteBusy = [bool]($flags -band 4)
    HostWroteComplete = [bool]($flags -band 2)
    BridgeSawFlushQueued = [bool]($flags -band 1)
    CpuCommandActive = [bool]($word -band 0x800)
    CpuFlushRequestHigh = [bool]($word -band 0x400)
    CpuAckInput = [bool]($word -band 0x200)
    CpuDoneInput = [bool]($word -band 0x100)
    PublicStatus = '{0:X2}' -f ($word -band 255)
}
