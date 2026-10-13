# Public core identity is case-sensitive on the library server.
# Storage/platform IDs intentionally remain lowercase chiskit.
function Resolve-ChisKitBaselineFile([string]$SdRoot, [string]$Relative) {
    # Read old packages without renaming them or invalidating their manifests.
    if ($Relative -cmatch '^Cores/Alius\.ChisKit/') {
        $dirs = @(Get-ChildItem -LiteralPath (Join-Path $SdRoot 'Cores') -Directory | Where-Object { $_.Name -ieq 'Alius.ChisKit' })
        if ($dirs.Count -ne 1) { throw 'Ambiguous baseline core directory' }
        return Join-Path $dirs[0].FullName $Relative.Substring('Cores/Alius.ChisKit/'.Length)
    }
    return Join-Path $SdRoot $Relative
}

function Initialize-ChisKitCoreDirectory([string]$SdRoot) {
    $parent = Join-Path $SdRoot 'Cores'
    $matches = @(Get-ChildItem -LiteralPath $parent -Directory | Where-Object { $_.Name -ieq 'Alius.ChisKit' })
    if ($matches.Count -ne 1) { throw "Expected exactly one ChisKit core under $parent" }
    if ($matches[0].Name -cne 'Alius.ChisKit') {
        $temporary = Join-Path $parent ('.chiskit-case-' + [guid]::NewGuid().ToString('N'))
        Move-Item -LiteralPath $matches[0].FullName -Destination $temporary
        Move-Item -LiteralPath $temporary -Destination (Join-Path $parent 'Alius.ChisKit')
    }
}

function Assert-ChisKitPackageLayout([string]$SdRoot) {
    $dirs = @(Get-ChildItem -LiteralPath (Join-Path $SdRoot 'Cores') -Directory)
    $matches = @($dirs | Where-Object { $_.Name -ieq 'Alius.ChisKit' })
    if ($matches.Count -ne 1 -or $matches[0].Name -cne 'Alius.ChisKit') { throw 'Noncanonical ChisKit core directory' }
    $metadata = (Get-Content -LiteralPath (Join-Path $matches[0].FullName 'core.json') -Raw | ConvertFrom-Json).core.metadata
    if ("$($metadata.author).$($metadata.shortname)" -cne 'Alius.ChisKit') { throw 'Core directory and metadata identity disagree' }
}
