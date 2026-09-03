import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


class MiniV3SsidToolsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.menu = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text()
        cls.scan = (ROOT / "esp32_marauder" / "WiFiScan.cpp").read_text()
        cls.header = (ROOT / "esp32_marauder" / "WiFiScan.h").read_text()

    def test_grouped_ssid_workflow_is_wired_to_mini_v3_menu(self):
        for marker in (
            '"Scan SSIDs"',
            '"Select SSIDs"',
            '"SSID Finder"',
            '"SSID Beacon Clone"',
            '"SSID Group Deauth"',
            "buildSSIDGroupMenu(SSIDMenuMode::Deauth)",
            "SSIDMenuMode::FoxHunt",
            "const bool fox_hunt_mode",
        ):
            self.assertIn(marker, self.menu)

        self.assertIn("SSIDGroupSummary", self.menu)
        self.assertIn("selected_count == ap_count", self.menu)
        self.assertIn("access_point.essid == group_name", self.menu)

    def test_finder_tracks_only_group_channels_and_filters_rssi(self):
        for marker in (
            "SSID_FINDER_SAMPLE_WINDOW = 5",
            "ssid_finder_channels",
            "SSID_FINDER_SWITCH_MARGIN_DB = 6",
            "SSID_FINDER_SWITCH_CYCLES = 2",
            "recordSSIDFinderPacket",
            "evaluateSSIDFinderTarget",
            "runSSIDFinder(currentTime)",
            "subtype != 0x80 && subtype != 0x50",
        ):
            self.assertIn(marker, self.header + self.scan)

        self.assertIn("sorted_samples[finder_ap.sample_count / 2]", self.scan)
        self.assertIn("channel_count * SSID_FINDER_CHANNEL_DWELL_MS", self.scan)

    def test_group_clone_uses_stable_local_bssid_and_exact_ssid(self):
        self.assertIn("WIFI_ATTACK_SSID_GROUP_CLONE", self.header)
        self.assertIn("(custom_ssid.bssid[0] | 0x02) & 0xFE", self.scan)
        self.assertIn("custom_ssid.bssid[5] ^ 0x01", self.scan)
        self.assertIn("if (scan_mode == WIFI_ATTACK_AP_SPAM)\n    ssidLen = random", self.scan)


if __name__ == "__main__":
    unittest.main()
