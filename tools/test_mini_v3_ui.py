import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]


class MiniV3UiTests(unittest.TestCase):
    def test_shared_ui_primitives_are_available(self):
        source = (ROOT / "esp32_marauder" / "MiniV3Ui.h").read_text(
            encoding="utf-8"
        )
        for primitive in ("header", "panel", "metric", "progress", "footer"):
            self.assertIn(f"inline void {primitive}", source)

    def test_deauth_views_use_the_shared_layout(self):
        source = (ROOT / "esp32_marauder" / "WiFiScan.cpp").read_text(
            encoding="utf-8"
        )
        self.assertIn("void WiFiScan::drawDeauthStatus(uint8_t scan_mode)", source)
        self.assertIn('"STATION DEAUTH"', source)
        self.assertIn('"BROADCAST DEAUTH"', source)
        self.assertIn("void WiFiScan::drawCameraDeauthStatus()", source)
        self.assertIn('String("CAM TX ")', source)

    def test_packet_monitor_uses_compact_header(self):
        source = (ROOT / "esp32_marauder" / "WiFiScan.cpp").read_text(
            encoding="utf-8"
        )
        self.assertIn("void WiFiScan::drawPacketMonitorControls()", source)
        self.assertIn("void WiFiScan::drawPacketMonitorGraphs()", source)
        self.assertIn('"UP/DN CH  L EXIT"', source)

    def test_menu_cursor_keeps_the_selected_row_inside_the_viewport(self):
        source = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text(
            encoding="utf-8"
        )
        self.assertIn("void MenuFunctions::selectMiniMenuIndex", source)
        self.assertIn(
            "target_index >= menu_start_index + BUTTON_SCREEN_LIMIT", source
        )
        self.assertIn("target_index + 1 - BUTTON_SCREEN_LIMIT", source)
        self.assertIn("this->menu_start_index = starting_index", source)
        self.assertIn("const int16_t row_y = 22 + (b * 17);", source)
        self.assertIn("const int16_t row_height = 15;", source)


if __name__ == "__main__":
    unittest.main()
