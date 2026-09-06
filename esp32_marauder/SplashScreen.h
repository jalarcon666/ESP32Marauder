#pragma once

#include <Arduino.h>

#ifdef MARAUDER_MINI_V3
static constexpr uint16_t MARAUDER_ETERNAL_SPLASH_WIDTH = 128;
static constexpr uint16_t MARAUDER_ETERNAL_SPLASH_HEIGHT = 128;

// The supplied RGB565 asset uses this placement macro so it remains portable
// across the Arduino ESP32 targets that include the header.
#define MK_SIGNAL_FLASH PROGMEM
#include "MiniV3SplashLogo.h"
#undef MK_SIGNAL_FLASH

#define marauder_eternal_splash mk_signal
#endif
