# Marauder Eternal Mini V3 upstream

This branch tracks the Mini V3 firmware from:

- Repository: `https://github.com/n0vajay05/ESP32MarauderEternal-MiniV3.git`
- Branch: `master`
- Imported tag: `FirmwareUpdate1.15.5`
- Imported commit: `45fd3ed4005055f293577a6e38fb25f08861cf22`
- Previous baseline: `8683b92cdeb01ed344d1b4ab27526bb03944b407`
- Upstream source: `firmware/MarauderEternal`
- Local destination: `esp32_marauder`

The complete upstream `.cpp` and `.h` inventory is present, and the runtime
changes in `8683b92..45fd3ed` are imported. The upstream `MarauderEternal.ino`
is stored locally as `esp32_marauder.ino` so Arduino and
the existing build workflows keep their canonical sketch name. Release
binaries, the standalone Eternal flasher, and upstream repository workflows are
not copied because this fork already owns those deployment layers.

## Deliberate local adaptations

- Board selection remains build-flag driven. The Eternal Mini V3 profile still
  rejects a non-ESP32-C5 target, while the fork's other hardware targets remain
  available.
- LilyGo T-Dongle C5 support and its shared SPI/display/LED handling are kept.
- The 8 MB `mini-v3-c5-8m-ota-v1` partition is supplied by the installer and CI,
  rather than committed as a transient sketch-local `partitions.csv`.
- The ESP32-C5 raw-frame compatibility symbol uses the explicit 1.15.5 linker
  wrapper. Deauth transmissions additionally retain unique sequence numbers
  and bounded backoff for transient TX-queue pressure.
- Firmware metadata remains hardware-specific on non-Mini builds, and SD OTA
  identity validation is enforced for the Mini V3 image.

For the next update, fetch the Eternal repository and review the complete range
starting at the imported commit above. Update this file only after the source
inventory, tests, ESP32-C5 build, linked wrapper map, and factory image have all
been verified.
