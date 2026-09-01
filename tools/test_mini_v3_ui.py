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
        self.assertIn('MiniV3Ui::header(display_obj.tft, "DEAUTH MONITOR"', source)
        self.assertIn('MiniV3Ui::header(display_obj.tft, "CAMERA LINK"', source)
        self.assertIn("this->drawDeauthStatus(selected_targets);", source)

    def test_packet_monitor_uses_compact_header(self):
        source = (ROOT / "esp32_marauder" / "WiFiScan.cpp").read_text(
            encoding="utf-8"
        )
        self.assertIn('MiniV3Ui::header(display_obj.tft, "PACKET MONITOR"', source)
        self.assertIn("MiniV3Ui::kHeaderHeight", source)


if __name__ == "__main__":
    unittest.main()
