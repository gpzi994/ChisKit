# Internal Pocket OS CRC is separate from package SHA256 checksums.
# append_os_crc.py intentionally skips already-stamped images, so it cannot
# repair this trailer after a fixed-layout binary patch.

if (-not ('ChisBurner.Build.OsCrc32' -as [type])) {
    Add-Type -TypeDefinition @'
namespace ChisBurner.Build {
    public static class OsCrc32 {
        public static uint Compute(byte[] data, int count) {
            if (data == null || count < 0 || count > data.Length)
                throw new System.ArgumentOutOfRangeException("count");
            uint crc = 0xFFFFFFFFu;
            for (int i = 0; i < count; ++i) {
                crc ^= data[i];
                for (int bit = 0; bit < 8; ++bit)
                    crc = (crc >> 1) ^ ((crc & 1u) != 0 ? 0xEDB88320u : 0u);
            }
            return crc ^ 0xFFFFFFFFu;
        }
    }
}
'@
}

function Get-ChisBurnerOsIntegrity {
    param([Parameter(Mandatory)][byte[]]$Bytes)
    if ($Bytes.Length -lt 8) {
        throw 'OS image is too short for an OFC1 integrity trailer'
    }
    $trailerOffset = $Bytes.Length - 8
    if ([System.Text.Encoding]::ASCII.GetString($Bytes, $trailerOffset, 4) -cne 'OFC1') {
        throw 'OS image lacks the mandatory OFC1 integrity trailer'
    }
    $crcOffset = $Bytes.Length - 4
    [uint32]$stored = [uint32]$Bytes[$crcOffset] -bor
        ([uint32]$Bytes[$crcOffset + 1] -shl 8) -bor
        ([uint32]$Bytes[$crcOffset + 2] -shl 16) -bor
        ([uint32]$Bytes[$crcOffset + 3] -shl 24)
    [uint32]$computed = [ChisBurner.Build.OsCrc32]::Compute($Bytes, $trailerOffset)
    [pscustomobject]@{
        Length = $Bytes.Length
        CrcOffset = $crcOffset
        StoredCrc = $stored
        ComputedCrc = $computed
        CrcMatches = $stored -eq $computed
    }
}

function Assert-ChisBurnerOsIntegrity {
    param(
        [Parameter(Mandatory)][byte[]]$Bytes,
        [string]$Context = 'OS image'
    )
    $info = Get-ChisBurnerOsIntegrity -Bytes $Bytes
    if (-not $info.CrcMatches) {
        throw ('{0}: internal CRC mismatch, stored={1:x8}, actual={2:x8}' -f
            $Context, $info.StoredCrc, $info.ComputedCrc)
    }
    return $info
}

function Set-ChisBurnerOsCrc {
    param([Parameter(Mandatory)][byte[]]$Bytes)
    $info = Get-ChisBurnerOsIntegrity -Bytes $Bytes
    $result = [byte[]]$Bytes.Clone()
    for ($i = 0; $i -lt 4; ++$i) {
        $result[$info.CrcOffset + $i] = [byte](($info.ComputedCrc -shr (8 * $i)) -band 255)
    }
    $null = Assert-ChisBurnerOsIntegrity -Bytes $result -Context 'CRC-repaired OS'
    return ,$result
}

function Assert-ChisBurnerZipOsIntegrity {
    param([Parameter(Mandatory)][string]$ZipPath)
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $archive = [System.IO.Compression.ZipFile]::OpenRead($ZipPath)
    try {
        $entry = $archive.GetEntry('SD_ROOT/Assets/chisburner/common/os.bin')
        if (-not $entry) { throw 'ZIP is missing the packaged os.bin' }
        $stream = $entry.Open()
        $memory = New-Object System.IO.MemoryStream
        try {
            $stream.CopyTo($memory)
            return Assert-ChisBurnerOsIntegrity -Bytes $memory.ToArray() -Context $ZipPath
        } finally {
            $memory.Dispose()
            $stream.Dispose()
        }
    } finally {
        $archive.Dispose()
    }
}
