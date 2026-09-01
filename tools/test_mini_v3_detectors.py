import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


class MiniV3DetectorTests(unittest.TestCase):
    def test_combined_scout_and_activity_tools_are_mini_v3_guarded(self):
        scout = (ROOT / "esp32_marauder" / "WirelessDeviceScout.cpp").read_text()
        activity = (ROOT / "esp32_marauder" / "WirelessActivityTools.cpp").read_text()

        for source in (scout, activity):
            self.assertIn("defined(MARAUDER_MINI_V3)", source)
            self.assertIn("esp_wifi_set_promiscuous(false)", source)
            self.assertIn("NimBLEDevice::deinit(true)", source)
            self.assertIn("SPDX-License-Identifier: MIT", source)

        self.assertIn("PERSISTENCE_WINDOW_MS = 5000", scout)
        self.assertIn("BASELINE_SWEEPS = 3", activity)
        self.assertIn("never proof of a", activity)

    def test_ble_fingerprint_detectors_and_menu_entries_are_present(self):
        detector = (ROOT / "esp32_marauder" / "BLEDeviceDetectors.cpp").read_text()
        menu = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text()

        for name in (
            "Meshtastic",
            "MeshCore",
            "SmartTag",
            "Tile",
            "Axon",
            "IBeacon",
            "NyanBox",
        ):
            self.assertIn(f"DetectorType::{name}", detector + menu)

        for label in ("Device Scout", "Activity Scanner", "Jam Detector"):
            self.assertIn(f'"{label}"', menu)

    def test_nyanbox_attribution_is_shipped(self):
        notices = (ROOT / "licenses" / "THIRD_PARTY_NOTICES.md").read_text()
        self.assertIn("## nyanBOX", notices)
        self.assertIn("MIT License", notices)
        self.assertIn("Copyright (c) 2025 jbohack", notices)

    def test_camera_detector_is_passive_until_explicit_confirmation(self):
        header = (ROOT / "esp32_marauder" / "WiFiCameraDetector.h").read_text()
        detector = (ROOT / "esp32_marauder" / "WiFiCameraDetector.cpp").read_text()
        scan = (ROOT / "esp32_marauder" / "WiFiScan.cpp").read_text()
        menu = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text()

        self.assertIn("Passively identifies", header)
        self.assertIn("No frame is transmitted by this function", header)
        self.assertIn("selectDeauthTarget", detector)
        self.assertIn('drawString("AUTHORIZED TEST?"', detector)
        self.assertIn("accepted = true", detector)
        self.assertIn("WiFiCameraDetector::selectDeauthTarget(target)", menu)

        self.assertIn("WIFI_ATTACK_CAMERA_DEAUTH", scan)
        self.assertIn("sendCameraDeauthFrame", scan)
        self.assertIn("stopping_camera_deauth", scan)
        self.assertIn("memset(&this->camera_deauth_targets", scan)

        for label in ("Camera Detect", "Camera Deauther"):
            self.assertIn(f'"{label}"', menu)

    def test_wifi_flock_detector_is_passive_and_attributed(self):
        header = (ROOT / "esp32_marauder" / "WiFiFlockDetector.h").read_text()
        detector = (ROOT / "esp32_marauder" / "WiFiFlockDetector.cpp").read_text()
        menu = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text()
        license_text = (ROOT / "licenses" / "FLOCK_YOU_LICENSE.txt").read_text()

        self.assertIn("receive-only Wi-Fi classification", header)
        self.assertIn("startPassiveScan", detector)
        self.assertIn("esp_wifi_set_promiscuous(true)", detector)
        self.assertNotIn("esp_wifi_80211_tx", detector)
        self.assertIn('"Detect Flock"', menu)
        self.assertIn("WiFiFlockDetector::run()", menu)
        self.assertIn("MIT License", license_text)


if __name__ == "__main__":
    unittest.main()
