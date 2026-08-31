import csv
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


class MiniV3HardwareTests(unittest.TestCase):
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
                "-Wl,--wrap=ieee80211_raw_frame_sanity_check",
                workflow,
            )

        self.assertFalse((ROOT / "esp32_marauder" / "partitions.csv").exists())


if __name__ == "__main__":
    unittest.main()
