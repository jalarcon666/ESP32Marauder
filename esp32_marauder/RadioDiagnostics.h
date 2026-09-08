#pragma once

#include <Arduino.h>
#include <esp_err.h>
#include <esp_wifi.h>

namespace RadioDiagnostics {

struct Snapshot {
  uint32_t startedMs;
  uint32_t rxPackets;
  uint32_t rxBytes;
  uint32_t rxManagement;
  uint32_t rxControl;
  uint32_t rxData;
  uint32_t rxMisc;
  int32_t rxRssiSum;
  int8_t rxStrongestRssi;
  int8_t rxWeakestRssi;
  int8_t rxLastRssi;
  uint32_t txAttempts;
  uint32_t txAccepted;
  uint32_t txFailed;
  uint32_t txBytes;
  int8_t requestedTxPowerQdbm;
  int8_t effectiveTxPowerQdbm;
  uint8_t effectiveTxPowerChannel;
  esp_err_t txPowerSetStatus;
  esp_err_t txPowerGetStatus;
  bool requestedTxPowerValid;
  bool effectiveTxPowerValid;
};

// Starts a new traffic-measurement interval without discarding the most recent
// TX power readback.
void resetTraffic(uint32_t nowMs);
void recordRx(const wifi_promiscuous_pkt_t* packet,
              wifi_promiscuous_pkt_type_t type);
void recordTx(esp_err_t status, size_t bytes);
void recordTxPower(int8_t requestedQdbm, esp_err_t setStatus,
                   int8_t effectiveQdbm, esp_err_t getStatus,
                   uint8_t channel);
void recordEffectiveTxPower(int8_t effectiveQdbm, esp_err_t getStatus,
                            uint8_t channel);
Snapshot snapshot();

}  // namespace RadioDiagnostics
