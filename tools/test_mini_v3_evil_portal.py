from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[1]
WIFI_SCAN = ROOT / "esp32_marauder" / "WiFiScan.cpp"
WIFI_SCAN_HEADER = ROOT / "esp32_marauder" / "WiFiScan.h"
EVIL_PORTAL = ROOT / "esp32_marauder" / "EvilPortal.cpp"
MINI_V3_WIFI6 = ROOT / "esp32_marauder" / "MiniV3WiFi6.cpp"


class MiniV3EvilPortalTests(unittest.TestCase):
    def test_softap_is_validated_before_portal_is_ready(self):
        source = EVIL_PORTAL.read_text(encoding="utf-8")
        self.assertIn("WiFi.softAPSSID() != String(apName)", source)
        self.assertIn("WiFi.softAPIP() != AP_IP", source)
        self.assertLess(
            source.index("WiFi.softAPSSID() != String(apName)"),
            source.index('Serial.println(F("Evil Portal READY"))'),
        )

    def test_deauth_waits_for_portal_startup(self):
        source = WIFI_SCAN.read_text(encoding="utf-8")
        self.assertIn("EVIL_PORTAL_STARTUP_GRACE_MS = 1500", source)
        self.assertIn(
            "initTime + EVIL_PORTAL_STARTUP_GRACE_MS", source
        )

    def test_deauth_rotates_all_selected_aps_and_pauses_for_clients(self):
        source = WIFI_SCAN.read_text(encoding="utf-8")
        header = WIFI_SCAN_HEADER.read_text(encoding="utf-8")
        self.assertIn(
            "bool WiFiScan::sendNextEvilPortalSelectedDeauth()", source
        )
        self.assertIn(
            "broadcast, this->evil_portal_deauth_cursor", source
        )
        self.assertIn("sendNextSelectedAPDeauth", source)
        self.assertIn("validDeauthChannel(access_point.channel)", source)
        self.assertIn("evil_portal_obj.getConnectedClientCount() > 0", source)
        self.assertIn("EVIL_PORTAL_CLIENT_REARM_MS", source)
        self.assertIn("bool sendNextEvilPortalSelectedDeauth();", header)
        self.assertIn("uint16_t evil_portal_deauth_cursor = 0;", header)

    def test_manual_portal_preserves_other_selected_networks(self):
        menu_source = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text(
            encoding="utf-8"
        )
        start = menu_source.index(
            "bool MenuFunctions::startEvilPortalForSSIDGroup"
        )
        end = menu_source.index(
            "bool MenuFunctions::startAutoEvilPortalForSSIDGroup", start
        )
        function = menu_source[start:end]
        self.assertNotIn("access_point.essid != group_name", function)
        self.assertIn("evil_portal_obj.setTargetAP", function)

    def test_status_exposes_client_pause(self):
        source = WIFI_SCAN.read_text(encoding="utf-8")
        self.assertIn('clients > 0 ? "PAUSED" : "ACTIVE"', source)
        self.assertIn('"Paused: client on AP"', source)

    def test_mini_v3_softaps_require_verified_wifi6(self):
        portal = EVIL_PORTAL.read_text(encoding="utf-8")
        scan = WIFI_SCAN.read_text(encoding="utf-8")
        helper = MINI_V3_WIFI6.read_text(encoding="utf-8")
        self.assertIn(
            "configureMiniV3SoftAPForTarget(this->target_ap_channel,", portal
        )
        self.assertIn(
            "configureMiniV3SoftAPForTarget(1, WIFI_GENERATION_6)", scan
        )
        self.assertIn("esp_wifi_set_protocols(WIFI_IF_AP, &requested)", helper)
        self.assertIn("esp_wifi_get_protocols(WIFI_IF_AP, &active)", helper)
        self.assertIn("WIFI_PROTOCOL_11AX", helper)
        self.assertIn("active_protocols != expected_protocols", helper)

    def test_evil_portal_mirrors_scanned_target_generation(self):
        portal = EVIL_PORTAL.read_text(encoding="utf-8")
        scan = WIFI_SCAN.read_text(encoding="utf-8")
        helper = MINI_V3_WIFI6.read_text(encoding="utf-8")
        self.assertIn("uint8_t detectWiFiGeneration", helper)
        self.assertIn("extension_id == 35 || extension_id == 36", helper)
        self.assertIn("ap.wifi_generation = detectWiFiGeneration(", scan)
        self.assertIn('obj.containsKey("wifi_generation")', scan)
        self.assertIn("WIFI_GENERATION_4", scan)
        self.assertIn("this->target_wifi_generation", portal)


if __name__ == "__main__":
    unittest.main()
