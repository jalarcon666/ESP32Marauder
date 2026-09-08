import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
FIRMWARE = ROOT / "esp32_marauder"


class MiniV3RfDiagnosticsTests(unittest.TestCase):
    def test_tx_power_request_is_verified_with_driver_readback(self):
        source = (FIRMWARE / "WiFiScan.cpp").read_text(encoding="utf-8")
        self.assertIn("MAX_WIFI_TX_POWER_QDBM = 82", source)
        self.assertIn("esp_wifi_set_max_tx_power(MAX_WIFI_TX_POWER_QDBM)", source)
        self.assertIn("esp_wifi_get_max_tx_power(&effective_tx_power)", source)
        self.assertIn("RadioDiagnostics::recordTxPower", source)
        self.assertIn("maximum 20 dBm 2.4 GHz cap confirmed", source)
        self.assertIn("maximum 18 dBm 5 GHz cap confirmed", source)
        self.assertIn("effectiveTxPowerChannel", source)

    def test_every_wifi_scan_raw_tx_is_accounted(self):
        source = (FIRMWARE / "WiFiScan.cpp").read_text(encoding="utf-8")
        direct_calls = source.count("esp_wifi_80211_tx(")
        self.assertEqual(direct_calls, 1)
        self.assertIn("RadioDiagnostics::recordTx(status", source)

    def test_cli_and_device_menu_expose_rf_diagnostics(self):
        command_header = (FIRMWARE / "CommandLine.h").read_text(encoding="utf-8")
        command_source = (FIRMWARE / "CommandLine.cpp").read_text(encoding="utf-8")
        menu_source = (FIRMWARE / "MenuFunctions.cpp").read_text(encoding="utf-8")
        self.assertIn('RF_DIAG_CMD[] = "rfdiag"', command_header)
        self.assertIn("wifi_scan_obj.RunRadioDiagnostics(false", command_source)
        self.assertIn('"RF Diagnostics"', menu_source)
        self.assertIn("wifi_scan_obj.RunRadioDiagnostics();", menu_source)

    def test_rx_callbacks_feed_shared_observation_counters(self):
        expected_sources = (
            "WiFiScan.cpp",
            "DroneRemoteID.cpp",
            "WiFiCameraDetector.cpp",
            "WiFiFlockDetector.cpp",
            "WirelessActivityTools.cpp",
            "WirelessDeviceScout.cpp",
        )
        for filename in expected_sources:
            with self.subTest(filename=filename):
                source = (FIRMWARE / filename).read_text(encoding="utf-8")
                self.assertIn("RadioDiagnostics::recordRx", source)


if __name__ == "__main__":
    unittest.main()
