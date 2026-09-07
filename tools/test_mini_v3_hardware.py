import csv
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


class MiniV3HardwareTests(unittest.TestCase):
    def test_local_build_script_requires_and_verifies_raw_frame_wrapper(self):
        script = (ROOT / "tools" / "build_mini_v3.ps1").read_text()
        self.assertIn("--wrap=ieee80211_raw_frame_sanity_check", script)
        self.assertIn(
            "ieee80211_raw_frame_sanity_check = "
            "__wrap_ieee80211_raw_frame_sanity_check",
            script,
        )
        self.assertIn("Select-String", script)
        self.assertIn("finally", script)
        self.assertIn("Remove-Item -LiteralPath $partitionTarget", script)
        self.assertIn("Adafruit_MAX1704X 1.0.2", script)
        self.assertIn("Adafruit_BusIO 1.15.0", script)
        self.assertIn("--libraries $LocalLibraries", script)

    def test_hardware_profile_matches_production_board(self):
        configs = (ROOT / "esp32_marauder" / "configs.h").read_text()
        setup = (ROOT / "User_Setup_marauder_mini_v3.h").read_text()

        for definition in (
            "#define TFT_MISO 2",
            "#define TFT_MOSI 7",
            "#define TFT_SCLK 6",
            "#define TFT_CS 23",
            "#define TFT_DC 24",
            "#define TFT_RST -1",
            "#define TFT_BL 5",
            "#define TOUCH_CS -1",
        ):
            self.assertIn(definition, configs)
            self.assertIn(definition, setup)

        for definition in (
            "#define SD_CS 10",
            "#define GPS_SERIAL_INDEX 1",
            "#define GPS_TX 14",
            "#define GPS_RX 13",
            "#define L_BTN 0",
            "#define C_BTN 1",
            "#define U_BTN 8",
            "#define R_BTN 9",
            "#define D_BTN 4",
        ):
            self.assertIn(definition, configs)

        self.assertIn('#define ST7735_DRIVER', setup)
        self.assertIn('#define TFT_BACKLIGHT_ON LOW', setup)
        self.assertIn('#define SPI_FREQUENCY 20000000', setup)
        self.assertIn('MARAUDER_MINI_V3 requires the ESP32-C5 board target', configs)
        self.assertIn('Six modern menu cards fit', configs)
        self.assertIn('#define BUTTON_SCREEN_LIMIT 6', configs)

        board_features = configs.index("//// BOARD FEATURES")
        mini_start = configs.index("#ifdef MARAUDER_MINI_V3", board_features)
        mini_end = configs.index("#endif", mini_start)
        mini_features = configs[mini_start:mini_end]
        self.assertIn("#define HAS_BATTERY", mini_features)

        battery_features = configs.index("//// BATTERY STUFF")
        mini_battery_start = configs.index(
            "#elif defined(MARAUDER_MINI_V3)", battery_features
        )
        mini_battery_end = configs.index("#elif", mini_battery_start + 1)
        mini_battery = configs[mini_battery_start:mini_battery_end]
        for definition in (
            "#define I2C_SDA 0",
            "#define I2C_SCL 1",
            "#define HAS_MAX1704X",
            "#define BATTERY_I2C_SHARED_BUTTONS",
        ):
            self.assertIn(definition, mini_battery)

        battery = (ROOT / "esp32_marauder" / "BatteryInterface.cpp").read_text()
        menu = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text()
        self.assertIn("digitalRead(I2C_SDA) == LOW", battery)
        self.assertIn("digitalRead(I2C_SCL) == LOW", battery)
        self.assertIn("Battery: MAX17048 not detected at 0x36", battery)
        self.assertIn("drawMiniBatteryStatus", menu)
        self.assertIn("if (current_menu != &mainMenu)", menu)
        self.assertIn("else if (gps_obj.getFixStatus())", menu)

    def test_custom_partition_layout_is_wired_into_every_build(self):
        partition_file = ROOT / "installer" / "partitions" / "mini_v3.csv"
        rows = [
            row
            for row in csv.reader(partition_file.read_text().splitlines())
            if row and not row[0].lstrip().startswith("#")
        ]
        normalized = [[column.strip() for column in row] for row in rows]
        self.assertEqual(
            normalized,
            [
                ["nvs", "data", "nvs", "0x9000", "0x5000", ""],
                ["otadata", "data", "ota", "0xe000", "0x2000", ""],
                ["app0", "app", "ota_0", "0x10000", "0x3C0000", ""],
                ["spiffs", "data", "spiffs", "0x3D0000", "0x20000", ""],
                ["coredump", "data", "coredump", "0x3F0000", "0x10000", ""],
                ["app1", "app", "ota_1", "0x400000", "0x3C0000", ""],
            ],
        )

        for workflow_name in (
            "build_parallel.yml",
            "nightly_build.yml",
            "build_installer_manifests.yml",
        ):
            workflow = (ROOT / ".github" / "workflows" / workflow_name).read_text()
            self.assertIn("if: matrix.board.flag == 'MARAUDER_MINI_V3'", workflow)
            self.assertIn(
                "cp installer/partitions/mini_v3.csv esp32_marauder/partitions.csv",
                workflow,
            )
            self.assertIn(
                "--wrap=ieee80211_raw_frame_sanity_check",
                workflow,
            )

        self.assertFalse((ROOT / "esp32_marauder" / "partitions.csv").exists())

    def test_c5_deauth_uses_direct_override_and_unique_sequences(self):
        scan_cpp = (ROOT / "esp32_marauder" / "WiFiScan.cpp").read_text()
        scan_h = (ROOT / "esp32_marauder" / "WiFiScan.h").read_text()

        self.assertIn('extern "C" int ieee80211_raw_frame_sanity_check(', scan_cpp)
        self.assertIn("__wrap_ieee80211_raw_frame_sanity_check", scan_cpp)
        self.assertIn("transmitPreparedDeauthFrame", scan_cpp + scan_h)
        self.assertIn("deauth_sequence++", scan_cpp)
        self.assertIn("sequence_control >> 8", scan_cpp)
        self.assertIn("deauth_tx_failures", scan_cpp + scan_h)
        self.assertIn("ESP_ERR_WIFI_WOULD_BLOCK", scan_cpp)
        self.assertIn("delayMicroseconds(250)", scan_cpp)
        self.assertIn("DEAUTH_TX_INTERVAL_MS", scan_cpp)
        self.assertIn("deauth_last_error_ms", scan_cpp + scan_h)


if __name__ == "__main__":
    unittest.main()
