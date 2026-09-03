import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = ROOT / "esp32_marauder"
TRACKING = ROOT / "ETERNAL_UPSTREAM.md"


class MiniV3UpstreamTests(unittest.TestCase):
    def test_eternal_1_15_5_baseline_is_recorded(self):
        tracking = TRACKING.read_text(encoding="utf-8")
        configs = (SOURCE / "configs.h").read_text(encoding="utf-8")
        self.assertIn("FirmwareUpdate1.15.5", tracking)
        self.assertIn("45fd3ed4005055f293577a6e38fb25f08861cf22", tracking)
        self.assertIn("8683b92cdeb01ed344d1b4ab27526bb03944b407", tracking)
        self.assertIn('#define MARAUDER_VERSION "v1.15.5"', configs)
        checker = (ROOT / "tools" / "check_eternal_upstream.ps1").read_text(
            encoding="utf-8"
        )
        self.assertIn("git ls-remote", checker)
        self.assertIn("refs/heads/master", checker)

    def test_new_1_15_5_runtime_modules_are_present(self):
        for name in (
            "DeauthFrame.cpp",
            "DeauthFrame.h",
            "DeviceClock.cpp",
            "DeviceClock.h",
            "RsnCapabilities.cpp",
            "RsnCapabilities.h",
            "SdTransferPath.cpp",
            "SdTransferPath.h",
            "StoragePaths.h",
            "UtcTime.cpp",
            "UtcTime.h",
        ):
            self.assertTrue((SOURCE / name).is_file(), name)

    def test_1_15_5_integration_contracts_are_wired(self):
        command_line = (SOURCE / "CommandLine.cpp").read_text(encoding="utf-8")
        sketch = (SOURCE / "esp32_marauder.ino").read_text(encoding="utf-8")
        settings = (SOURCE / "settings.cpp").read_text(encoding="utf-8")
        wifi_scan = (SOURCE / "WiFiScan.cpp").read_text(encoding="utf-8")
        headers = (SOURCE / "WiFiScan.h").read_text(encoding="utf-8") + (
            SOURCE / "SDInterface.h"
        ).read_text(encoding="utf-8")
        for marker in (
            "sdSessionActive",
            "SdTransferPath",
            "StoragePaths",
        ):
            self.assertIn(marker, command_line + sketch + wifi_scan + headers)
        self.assertIn("_commitDocument", settings)
        self.assertIn('SPIFFS.rename("/settings.json", "/settings.backup")', settings)
        self.assertIn("DeviceClock device_clock_obj", sketch)
        self.assertIn("sendNextSelectedAPDeauth", wifi_scan)
        self.assertIn("sendNextSelectedStationDeauth", wifi_scan)


if __name__ == "__main__":
    unittest.main()
