import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = ROOT / "esp32_marauder"


class MiniV3MetadataTests(unittest.TestCase):
    def test_firmware_identity_is_embedded(self):
        metadata = (SOURCE / "FirmwareMetadata.cpp").read_text(encoding="utf-8")
        configs = (SOURCE / "configs.h").read_text(encoding="utf-8")
        self.assertIn('"Marauder Eternal Mini V3"', metadata)
        self.assertIn("MARAUDER_VERSION", metadata)
        self.assertIn("MARAUDER_PARTITION_LAYOUT", metadata)
        self.assertIn('"mini-v3-c5-8m-ota-v1"', configs)

    def test_sd_ota_rejects_wrong_or_incomplete_images(self):
        source = (SOURCE / "SDInterface.cpp").read_text(encoding="utf-8")
        header = (SOURCE / "SDInterface.h").read_text(encoding="utf-8")
        self.assertIn("FirmwareMetadata.h", header)
        self.assertIn("validateUpdate", source)
        self.assertIn("ESP32_C5_IMAGE_ID", source)
        self.assertIn("#ifndef MARAUDER_MINI_V3", source)
        self.assertIn("MarauderFirmware::metadataMatches", source)
        self.assertIn("Update.abort()", source)
        self.assertNotIn("esp_ota_set_boot_partition(next)", source)


if __name__ == "__main__":
    unittest.main()
