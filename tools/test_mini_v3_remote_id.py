import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = ROOT / "esp32_marauder" / "DroneRemoteID.cpp"
HEADER = ROOT / "esp32_marauder" / "DroneRemoteID.h"
SPOOFER_SOURCE = ROOT / "esp32_marauder" / "DroneRemoteIDSpoofer.cpp"
SPOOFER_HEADER = ROOT / "esp32_marauder" / "DroneRemoteIDSpoofer.h"


class MiniV3RemoteIdTests(unittest.TestCase):
    def test_receiver_covers_all_passive_transports(self):
        source = SOURCE.read_text(encoding="utf-8")
        for decoder in (
            "decodeWifiBeacon",
            "decodeWifiNan",
            "decodeBleAdvertisement",
        ):
            self.assertIn(decoder, source)

    def test_transport_parsers_check_frame_bounds(self):
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("if (fieldEnd > length)", source)
        self.assertIn("if (fieldEnd > payload.size())", source)
        self.assertIn("if (attributeEnd > length", source)
        self.assertIn("if (!message || length < ODID_MESSAGE_SIZE)", source)

    def test_passive_receiver_and_active_broadcaster_are_both_exposed(self):
        header = HEADER.read_text(encoding="utf-8")
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("void run();", header)
        self.assertIn("struct CapturedDrone", header)
        self.assertIn("selectCapturedForSpoof", header)
        self.assertIn("selectCapturedForSpoof", source)
        self.assertTrue(SPOOFER_HEADER.is_file())
        self.assertTrue(SPOOFER_SOURCE.is_file())

    def test_broadcaster_requires_target_selection_and_authorization(self):
        spoofer = SPOOFER_SOURCE.read_text(encoding="utf-8")
        menu = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text(
            encoding="utf-8"
        )
        self.assertIn("selectTarget", spoofer)
        self.assertIn('"Drone Spoof"', menu)
        self.assertIn('"Run (Authorized)"', menu)
        self.assertIn("DroneRemoteIDSpoofer::run();", menu)

    def test_menu_entry_is_available(self):
        menu = (ROOT / "esp32_marauder" / "MenuFunctions.cpp").read_text(
            encoding="utf-8"
        )
        self.assertIn('"Drone Remote ID"', menu)
        self.assertIn("DroneRemoteID::run();", menu)


if __name__ == "__main__":
    unittest.main()
