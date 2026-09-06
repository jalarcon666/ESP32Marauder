#include "MiniV3WiFi6.h"

#include "configs.h"

uint8_t detectWiFiGeneration(const uint8_t* information_elements,
                             size_t length) {
  if (information_elements == nullptr)
    return WIFI_GENERATION_LEGACY;

  bool has_ht = false;
  bool has_vht = false;
  bool has_he = false;
  size_t position = 0;
  while (position + 2 <= length) {
    const uint8_t element_id = information_elements[position];
    const size_t element_length = information_elements[position + 1];
    position += 2;
    if (element_length > length - position)
      break;

    has_ht = has_ht || element_id == 45 || element_id == 61;
    has_vht = has_vht || element_id == 191 || element_id == 192;
    if (element_id == 255 && element_length >= 1) {
      const uint8_t extension_id = information_elements[position];
      has_he = has_he || extension_id == 35 || extension_id == 36;
    }
    position += element_length;
  }

  if (has_he)
    return WIFI_GENERATION_6;
  if (has_vht)
    return WIFI_GENERATION_5;
  if (has_ht)
    return WIFI_GENERATION_4;
  return WIFI_GENERATION_LEGACY;
}

#if defined(MARAUDER_MINI_V3) && defined(ARDUINO_ESP32C5_DEV)
  #include "esp_err.h"
  #include "esp_wifi.h"

bool configureMiniV3SoftAPForTarget(uint8_t channel,
                                    uint8_t wifi_generation) {
  wifi_protocols_t requested{};
  requested.ghz_2g = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G;
  requested.ghz_5g = WIFI_PROTOCOL_11A;
  if (wifi_generation >= WIFI_GENERATION_4) {
    requested.ghz_2g |= WIFI_PROTOCOL_11N;
    requested.ghz_5g |= WIFI_PROTOCOL_11N;
  }
  if (wifi_generation >= WIFI_GENERATION_5)
    requested.ghz_5g |= WIFI_PROTOCOL_11AC;
  if (wifi_generation >= WIFI_GENERATION_6) {
    requested.ghz_2g |= WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX;
    requested.ghz_5g |= WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AC |
                        WIFI_PROTOCOL_11AX;
  }

  esp_err_t status = esp_wifi_set_protocols(WIFI_IF_AP, &requested);
  if (status != ESP_OK) {
    Serial.printf("SoftAP target protocol setup failed: %s\n",
                  esp_err_to_name(status));
    return false;
  }

  wifi_protocols_t active{};
  status = esp_wifi_get_protocols(WIFI_IF_AP, &active);
  if (status != ESP_OK) {
    Serial.printf("SoftAP target protocol verification failed: %s\n",
                  esp_err_to_name(status));
    return false;
  }

  const bool uses_5ghz = channel > 14;
  const uint16_t active_protocols =
      uses_5ghz ? active.ghz_5g : active.ghz_2g;
  const uint16_t expected_protocols =
      uses_5ghz ? requested.ghz_5g : requested.ghz_2g;
  if (active_protocols != expected_protocols) {
    Serial.printf("SoftAP target protocol mismatch on channel %u "
                  "(expected=0x%02x active=0x%02x)\n",
                  channel, expected_protocols, active_protocols);
    return false;
  }

  Serial.printf("SoftAP mirrors target Wi-Fi %u on channel %u "
                "(%s, protocols=0x%02x)\n",
                wifi_generation, channel,
                uses_5ghz ? "5 GHz" : "2.4 GHz", active_protocols);
  return true;
}

#else

bool configureMiniV3SoftAPForTarget(uint8_t channel,
                                    uint8_t wifi_generation) {
  (void)channel;
  (void)wifi_generation;
  return true;
}

#endif
