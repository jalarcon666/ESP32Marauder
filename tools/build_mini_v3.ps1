param(
  [string]$ArduinoCli = "C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe",
  [string]$BuildPath = "",
  [string]$ArduinoData = "",
  [string]$ArduinoDownloads = "",
  [string]$ReferenceLibraries = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$workspaceRoot = Split-Path -Parent $repoRoot
if ($BuildPath -eq "") {
  $BuildPath = Join-Path $workspaceRoot "builds\mini-v3\working"
}
if ($ArduinoData -eq "") {
  $ArduinoData = Join-Path $workspaceRoot ".arduino-mini-v3\data"
}
if ($ArduinoDownloads -eq "") {
  $ArduinoDownloads = Join-Path $workspaceRoot ".arduino-mini-v3\downloads"
}
if ($ReferenceLibraries -eq "") {
  $ReferenceLibraries = Join-Path $workspaceRoot "ReferenceMiniV3\libraries"
}

$partitionSource = Join-Path $repoRoot "installer\partitions\mini_v3.csv"
$partitionTarget = Join-Path $repoRoot "esp32_marauder\partitions.csv"
$sketchPath = Join-Path $repoRoot "esp32_marauder"
$repoLibraries = Join-Path $repoRoot "libraries"
$mapPath = Join-Path $BuildPath "esp32_marauder.ino.map"
$wrapperAssignment =
  "ieee80211_raw_frame_sanity_check = __wrap_ieee80211_raw_frame_sanity_check"

if (Test-Path -LiteralPath $partitionTarget) {
  throw "Refusing to overwrite existing partition file: $partitionTarget"
}

New-Item -ItemType Directory -Path $BuildPath -Force | Out-Null
Copy-Item -LiteralPath $partitionSource -Destination $partitionTarget
try {
  $env:ARDUINO_DIRECTORIES_DATA = $ArduinoData
  $env:ARDUINO_DIRECTORIES_DOWNLOADS = $ArduinoDownloads

  & $ArduinoCli compile `
    --fqbn "esp32:esp32:esp32c5:FlashSize=8M,PartitionScheme=custom,PSRAM=enabled" `
    --build-property "compiler.cpp.extra_flags=-DMARAUDER_MINI_V3" `
    --build-property "compiler.c.elf.extra_flags=-Wl,--wrap=ieee80211_raw_frame_sanity_check -Wl,--defsym=ieee80211_raw_frame_sanity_check=__wrap_ieee80211_raw_frame_sanity_check" `
    --libraries $ReferenceLibraries `
    --libraries $repoLibraries `
    --build-path $BuildPath `
    $sketchPath

  if ($LASTEXITCODE -ne 0) {
    throw "Mini V3 compilation failed with exit code $LASTEXITCODE"
  }
  if (!(Test-Path -LiteralPath $mapPath) -or
      !(Select-String -LiteralPath $mapPath -SimpleMatch $wrapperAssignment -Quiet)) {
    throw "Mini V3 raw-frame wrapper is missing from the linker map"
  }

  Write-Host "Mini V3 build complete with raw-frame TX wrapper verified."
  Write-Host (Join-Path $BuildPath "esp32_marauder.ino.merged.bin")
}
finally {
  Remove-Item -LiteralPath $partitionTarget -Force -ErrorAction SilentlyContinue
}
