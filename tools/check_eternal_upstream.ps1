param(
    [switch]$FailIfBehind
)

$ErrorActionPreference = "Stop"
$repository = "https://github.com/n0vajay05/ESP32MarauderEternal-MiniV3.git"
$trackingFile = Join-Path $PSScriptRoot "..\ETERNAL_UPSTREAM.md"
$tracking = Get-Content -LiteralPath $trackingFile -Raw
$match = [regex]::Match($tracking, 'Imported commit: `([0-9a-f]{40})`')
if (-not $match.Success) {
    throw "ETERNAL_UPSTREAM.md does not contain a valid imported commit"
}

$trackedCommit = $match.Groups[1].Value
$remoteLine = git ls-remote $repository refs/heads/master
if ($LASTEXITCODE -ne 0 -or -not $remoteLine) {
    throw "Unable to read Marauder Eternal upstream"
}

$remoteCommit = ($remoteLine -split "\s+")[0]
if ($remoteCommit -eq $trackedCommit) {
    Write-Host "Marauder Eternal is current at $trackedCommit"
    exit 0
}

Write-Warning "Marauder Eternal changed: tracked=$trackedCommit remote=$remoteCommit"
Write-Host "Review the complete range $trackedCommit..$remoteCommit before updating the manifest."
if ($FailIfBehind) {
    exit 1
}
