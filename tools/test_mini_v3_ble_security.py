import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = ROOT / "esp32_marauder" / "BLESecurityTools.cpp"
HEADER = ROOT / "esp32_marauder" / "BLESecurityTools.h"
COMPANIES = ROOT / "esp32_marauder" / "BLECompanyIdentifiers.cpp"
SCAN_HEADER = ROOT / "esp32_marauder" / "WiFiScan.h"
SCAN_SOURCE = ROOT / "esp32_marauder" / "WiFiScan.cpp"
MENU = ROOT / "esp32_marauder" / "MenuFunctions.cpp"


class MiniV3BleSecurityTests(unittest.TestCase):
    def test_eternal_ble_tool_modules_are_present(self):
        self.assertTrue(SOURCE.is_file())
        self.assertTrue(HEADER.is_file())
        self.assertTrue(COMPANIES.is_file())
        source = SOURCE.read_text(encoding="utf-8")
        for api in (
            "showAdvertisedInfo",
            "inspectTarget",
            "runDeviceSpoof",
            "deviceDisplayLabel",
        ):
            self.assertIn(api, source)

    def test_scan_model_retains_full_advertisement_metadata(self):
        header = SCAN_HEADER.read_text(encoding="utf-8")
        for field in (
            "advertisedServices",
            "addressType",
            "companyId",
            "appearance",
            "connectable",
            "scannable",
            "advertisementData",
            "scanResponseData",
            "advertisementTruncated",
        ):
            self.assertIn(field, header)

    def test_sd_capture_has_start_capture_and_stop_paths(self):
        header = SCAN_HEADER.read_text(encoding="utf-8")
        source = SCAN_SOURCE.read_text(encoding="utf-8")
        self.assertIn("BT_SCAN_ADVERTISEMENT_CAPTURE", header)
        self.assertIn("startBLEAdvertisementCapture", source)
        self.assertIn("captureBLEAdvertisement", source)
        self.assertIn("buffer_obj.logOpen", source)
        self.assertIn("buffer_obj.save()", source)
        self.assertGreaterEqual(
            source.count("BT_SCAN_ADVERTISEMENT_CAPTURE"), 8
        )

    def test_menu_exposes_passive_and_active_tools(self):
        menu = MENU.read_text(encoding="utf-8")
        for label in (
            '"BLE Discovery"',
            '"Scan Targets"',
            '"Capture Adverts to SD"',
            '"Select Target"',
            '"Advertised Info"',
            '"GATT Enumeration"',
            '"Device Spoof"',
        ):
            self.assertIn(label, menu)
        self.assertIn('"Authorize Device Spoof"', menu)
        self.assertIn('"Run (Authorized)"', menu)


if __name__ == "__main__":
    unittest.main()
