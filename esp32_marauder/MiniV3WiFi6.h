#pragma once

#include <Arduino.h>

enum WiFiGeneration : uint8_t {
  WIFI_GENERATION_LEGACY = 0,
  WIFI_GENERATION_4 = 4,
  WIFI_GENERATION_5 = 5,
  WIFI_GENERATION_6 = 6,
};

// Read HT, VHT and HE information elements from a beacon and return the
// highest Wi-Fi generation advertised by the target AP.
uint8_t detectWiFiGeneration(const uint8_t* information_elements,
                             size_t length);

// Restore and verify the ESP32-C5 SoftAP protocols after Marauder modes that
// temporarily use raw/long-range Wi-Fi configurations. The active band mirrors
// the target AP generation, including 802.11ax whenever the target advertises
// HE capabilities.
bool configureMiniV3SoftAPForTarget(uint8_t channel,
                                    uint8_t wifi_generation);
