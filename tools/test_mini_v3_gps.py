import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


class MiniV3GpsTests(unittest.TestCase):
    def test_mini_v3_uses_non_blocking_multibaud_recovery(self):
        source = (ROOT / "esp32_marauder" / "GpsInterface.cpp").read_text()
        header = (ROOT / "esp32_marauder" / "GpsInterface.h").read_text()

        self.assertIn("115200, 9600, 38400, 57600, 19200, 4800", source)
        self.assertIn("RECOVERY_BAUD_DWELL_MS = 1800", source)
        self.assertIn("GPS_TRAFFIC_TIMEOUT_MS = 6000", source)
        self.assertIn("GPS_FIX_TIMEOUT_MS = 3500", source)
        self.assertIn("MicroNMEA::testChecksum(sentence)", source)
        self.assertIn("if (nmea.process(c))", source)
        self.assertIn("void GpsInterface::listenAtBaud", source)
        self.assertIn("uint32_t getLastSentenceAgeMs();", header)

        begin_start = source.index("void GpsInterface::begin()")
        begin_end = source.index("void GpsInterface::listenAtBaud", begin_start)
        begin_body = source[begin_start:begin_end]
        self.assertNotIn("delay(", begin_body)
        self.assertNotIn("$PSTMSAVEPAR", begin_body)
        self.assertIn("rotate in main()", begin_body)

    def test_gps_menu_and_diagnostics_remain_available_while_searching(self):
        menu = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text()
        wifi_scan = (ROOT / "esp32_marauder" / "WiFiScan.cpp").read_text()

        for label in ('"GPS Data"', '"NMEA Stream"', '"GPS Tracker"',
                      '"GPS POI"'):
            self.assertIn(label, menu)
        self.assertIn("const bool gps_locked = gps_obj.getFixStatus();", menu)
        self.assertIn("if (gps_locked)", menu)

        self.assertIn('F("  NMEA: Searching")', wifi_scan)
        self.assertIn('"  Baud: " + String(gps_obj.getBaudRate())', wifi_scan)
        self.assertIn("gps_obj.getLastSentenceAgeMs()", wifi_scan)


if __name__ == "__main__":
    unittest.main()
