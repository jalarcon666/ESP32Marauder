from pathlib import Path
import hashlib
import re
import unittest


ROOT = Path(__file__).resolve().parents[1]
FIRMWARE = ROOT / "esp32_marauder"


class MiniV3LogoAndBeaconTests(unittest.TestCase):
    def test_supplied_128_square_logo_is_used(self):
        logo = FIRMWARE / "MiniV3SplashLogo.h"
        wrapper = (FIRMWARE / "SplashScreen.h").read_text(encoding="utf-8")
        contents = logo.read_text(encoding="utf-8")
        self.assertIn("mk_signal[16384]", contents)
        self.assertEqual(len(re.findall(r"0x[0-9A-Fa-f]{4}", contents)), 16384)
        self.assertEqual(
            hashlib.sha256(logo.read_bytes()).hexdigest().upper(),
            "3F146D3B07279095D07645F0A34F686DFCEFEC507E4968CD6109390F01952CE0",
        )
        self.assertIn('#include "MiniV3SplashLogo.h"', wrapper)
        self.assertIn("#define marauder_eternal_splash mk_signal", wrapper)

    def test_mini_startup_hides_attribution_and_main_menu_uses_port_name(self):
        sketch = (FIRMWARE / "esp32_marauder.ino").read_text(encoding="utf-8")
        menu = (FIRMWARE / "MenuFunctions.cpp").read_text(encoding="utf-8")
        mini_start = sketch.index("#ifdef MARAUDER_MINI_V3")
        mini_end = sketch.index("#else", mini_start)
        mini_splash = sketch[mini_start:mini_end]
        self.assertIn(
            'drawMiniV3SplashTitle("ESP32 MARAUDER MINI V3", 1, TFT_WHITE)',
            mini_splash,
        )
        self.assertNotIn('drawSplashText("ETERNAL"', mini_splash)
        self.assertNotIn('drawSplashText("Version " MARAUDER_VERSION', mini_splash)
        self.assertNotIn("JustCallMeKoKo", mini_splash)
        self.assertNotIn("n0vajay05", mini_splash)
        self.assertEqual(
            menu.count('current_menu == &mainMenu ? "Marauder Mini V3" :'),
            2,
        )
        self.assertNotIn('current_menu == &mainMenu ? ""', menu)
        self.assertNotIn(
            'current_menu == &mainMenu ? "Marauder Eternal"', menu
        )

    def test_html_selector_lives_under_evil_portal_only(self):
        source = (FIRMWARE / "MenuFunctions.cpp").read_text(encoding="utf-8")
        evil_start = source.index("evilPortalMenu.parentMenu")
        general_start = source.index("// Build WiFi General menu", evil_start)
        evil_block = source[evil_start:general_start]
        general_end = source.index("bluetoothMenu.parentMenu", general_start)
        general_block = source[general_start:general_end]
        self.assertIn('addNodes(&evilPortalMenu, "Select EP HTML File"', evil_block)
        self.assertIn("htmlMenu.parentMenu = &evilPortalMenu", evil_block)
        self.assertNotIn('addNodes(&wifiGeneralMenu, "Select EP HTML File"', general_block)

    def test_preset_beacons_send_only_the_real_frame(self):
        source = (FIRMWARE / "WiFiScan.cpp").read_text(encoding="utf-8")
        start = source.index("void WiFiScan::broadcastSetSSID")
        end = source.index("void WiFiScan::broadcastRandomSSID", start)
        function = source[start:end]
        self.assertIn("const size_t frame_length = 51 + ssidLen", function)
        self.assertIn(
            "esp_wifi_80211_tx(WIFI_IF_AP, packet, frame_length, false)",
            function,
        )
        self.assertNotIn("packet, sizeof(packet)", function)
        self.assertIn("(random(256) & 0xFC) | 0x02", function)
        self.assertIn("if (last_status == ESP_OK)", function)
        self.assertIn("this->beacon_tx_failures++", function)

    def test_rick_and_funny_use_bounded_channel_dwell_scheduler(self):
        source = (FIRMWARE / "WiFiScan.cpp").read_text(encoding="utf-8")
        self.assertIn("PRESET_BEACON_TX_INTERVAL_MS = 12", source)
        self.assertIn("PRESET_BEACON_CHANNEL_DWELL_MS = 1000", source)
        self.assertIn("static constexpr uint8_t channels[] = {1, 6, 11}", source)
        self.assertIn("currentTime, rick_roll", source)
        self.assertIn("currentTime, funny_beacon", source)
        self.assertNotIn("for (int i = 0; i < 7; i++)", source)


if __name__ == "__main__":
    unittest.main()
