import hashlib
import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE_ROOT = ROOT / "esp32_marauder"
REFERENCE_ROOT = ROOT.parent / "ReferenceMiniV3" / "firmware" / "MarauderEternal"

GAMES = (
    ("Snake", "SnakeGame"),
    ("Pong", "PongGame"),
    ("Connect 4", "ConnectFourGame"),
    ("Breakout", "BreakoutGame"),
    ("Tetris", "TetrisGame"),
    ("Space Invaders", "SpaceInvadersGame"),
    ("Missile Command", "MissileCommandGame"),
    ("8 Ball Billiards", "EightBallGame"),
    ("5 Card Draw", "FiveCardDrawGame"),
    ("Blackjack", "BlackjackGame"),
)


class MiniV3GamesTests(unittest.TestCase):
    def test_all_eternal_games_are_present_and_wired(self):
        menu = (SOURCE_ROOT / "MenuFunctions.cpp").read_text(encoding="utf-8")
        header = (SOURCE_ROOT / "MenuFunctions.h").read_text(encoding="utf-8")
        self.assertIn("Menu gamesMenu", header)
        self.assertIn('gamesMenu.name = "Games"', menu)
        for label, module in GAMES:
            self.assertTrue((SOURCE_ROOT / f"{module}.cpp").is_file())
            self.assertTrue((SOURCE_ROOT / f"{module}.h").is_file())
            self.assertIn(f'"{label}"', menu)
            self.assertIn(f"{module}::run();", menu)

    def test_shared_input_and_framebuffer_are_present(self):
        frame_buffer = (SOURCE_ROOT / "GameFrameBuffer.h").read_text(
            encoding="utf-8"
        )
        game_input = (SOURCE_ROOT / "GameInput.h").read_text(encoding="utf-8")
        self.assertIn("class GameFrameBuffer", frame_buffer)
        self.assertIn("class TapHoldButton", game_input)
        self.assertIn("waitForRelease", game_input)

    def test_game_sources_match_eternal_reference(self):
        if not REFERENCE_ROOT.is_dir():
            self.skipTest("local Eternal reference tree is unavailable")
        files = ["GameFrameBuffer.h", "GameInput.h"]
        for _, module in GAMES:
            files.extend((f"{module}.cpp", f"{module}.h"))
        for name in files:
            expected = hashlib.sha256((REFERENCE_ROOT / name).read_bytes()).digest()
            actual = hashlib.sha256((SOURCE_ROOT / name).read_bytes()).digest()
            self.assertEqual(expected, actual, name)


if __name__ == "__main__":
    unittest.main()
