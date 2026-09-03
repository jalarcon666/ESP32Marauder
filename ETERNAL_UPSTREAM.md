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

## Differences from Eternal 1.15.5

### ESP32-C5 raw transmission

- Eternal sends deauthentication frames with `en_sys_seq=false` while leaving
  the template's sequence-control field unchanged. This fork assigns a fresh
  12-bit sequence number to every prepared frame and resets the counter when a
  transmission session starts or stops.
- Eternal immediately continues after every `esp_wifi_80211_tx()` result. This
  fork waits 250 microseconds only after the transient `ESP_ERR_NO_MEM`,
  `ESP_ERR_WIFI_WOULD_BLOCK`, and `ESP_ERR_WIFI_TIMEOUT` results so the C5 Wi-Fi
  task can drain its queue. The helper does not retry or increase the requested
  transmission rate; the existing bounded scheduler remains responsible for
  subsequent attempts.
- Camera deauthentication uses the same address-validation and transmission
  helper as the other deauthentication modes. Per-camera counters are retained
  and the common attempted, accepted, and failed counters are updated as well.

### Fork functionality retained

- The Mini V3 interface layer, bounded menu viewport, active-scan screens, and
  TX health display remain enabled alongside the Eternal 1.15.5 menus.
- Passive Remote ID, Remote ID spoofing, the passive BLE suite, Wi-Fi/BLE Flock
  detection, camera detection, scoped camera deauthentication, and all ten
  Eternal games remain present.
- Hardware support removed from the Eternal Mini-V3-only repository remains in
  this multi-target fork. This includes the AXP192, Flipper, M5StickC, XIAO LED,
  touch-keyboard, and LilyGo T-Dongle C5 support modules.
- Local helper modules such as `MiniV3Ui`, `TargetListSort`, and `TDongleStats`
  are preserved. No upstream `.cpp` or `.h` file is missing from the local
  firmware inventory.

### Build and deployment

- The fork keeps its existing sketch name, installer manifests, CI matrix, and
  release layout. Eternal's standalone flasher, generated release binaries,
  and repository-specific workflows are therefore not vendored.
- Mini V3 builds use Arduino ESP32 core 3.3.4, DIO at 80 MHz, an 8 MB flash
  image, and offsets `0x2000`, `0x8000`, `0xe000`, and `0x10000`.
- The imported firmware source was compiled for ESP32-C5, the raw-frame wrapper
  was verified in the linker map, all 50 repository tests passed, and the
  resulting application image checksum and validation hash were accepted by
  `esptool`.

The firmware synchronization is recorded in commit `b914e4c`; upstream
tracking and regression coverage are recorded in commit `5d042bf`.

For the next update, fetch the Eternal repository and review the complete range
starting at the imported commit above. Update this file only after the source
inventory, tests, ESP32-C5 build, linked wrapper map, and factory image have all
been verified.
