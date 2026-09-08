#include "RadioDiagnostics.h"

namespace RadioDiagnostics {
namespace {

volatile uint32_t startedMs = 0;
volatile uint32_t rxPackets = 0;
volatile uint32_t rxBytes = 0;
volatile uint32_t rxManagement = 0;
volatile uint32_t rxControl = 0;
volatile uint32_t rxData = 0;
volatile uint32_t rxMisc = 0;
volatile int32_t rxRssiSum = 0;
volatile int32_t rxStrongestRssi = INT8_MIN;
volatile int32_t rxWeakestRssi = INT8_MAX;
volatile int32_t rxLastRssi = 0;
volatile uint32_t txAttempts = 0;
volatile uint32_t txAccepted = 0;
volatile uint32_t txFailed = 0;
volatile uint32_t txBytes = 0;
volatile int8_t requestedTxPowerQdbm = 0;
volatile int8_t effectiveTxPowerQdbm = 0;
volatile int32_t txPowerSetStatus = ESP_ERR_INVALID_STATE;
volatile int32_t txPowerGetStatus = ESP_ERR_INVALID_STATE;
volatile uint8_t txPowerFlags = 0;

template <typename T>
T loadRelaxed(const volatile T* value) {
  return __atomic_load_n(value, __ATOMIC_RELAXED);
}

template <typename T>
void storeRelaxed(volatile T* target, T value) {
  __atomic_store_n(target, value, __ATOMIC_RELAXED);
}

void updateStrongest(int8_t rssi) {
  int32_t current = loadRelaxed(&rxStrongestRssi);
  const int32_t candidate = rssi;
  while (candidate > current &&
         !__atomic_compare_exchange_n(&rxStrongestRssi, &current, candidate,
                                      true, __ATOMIC_RELAXED,
                                      __ATOMIC_RELAXED)) {}
}

void updateWeakest(int8_t rssi) {
  int32_t current = loadRelaxed(&rxWeakestRssi);
  const int32_t candidate = rssi;
  while (candidate < current &&
         !__atomic_compare_exchange_n(&rxWeakestRssi, &current, candidate,
                                      true, __ATOMIC_RELAXED,
                                      __ATOMIC_RELAXED)) {}
}

}  // namespace

void resetTraffic(uint32_t nowMs) {
  storeRelaxed(&startedMs, nowMs);
  storeRelaxed(&rxPackets, uint32_t{0});
  storeRelaxed(&rxBytes, uint32_t{0});
  storeRelaxed(&rxManagement, uint32_t{0});
  storeRelaxed(&rxControl, uint32_t{0});
  storeRelaxed(&rxData, uint32_t{0});
  storeRelaxed(&rxMisc, uint32_t{0});
  storeRelaxed(&rxRssiSum, int32_t{0});
  storeRelaxed(&rxStrongestRssi, int32_t{INT8_MIN});
  storeRelaxed(&rxWeakestRssi, int32_t{INT8_MAX});
  storeRelaxed(&rxLastRssi, int32_t{0});
  storeRelaxed(&txAttempts, uint32_t{0});
  storeRelaxed(&txAccepted, uint32_t{0});
  storeRelaxed(&txFailed, uint32_t{0});
  storeRelaxed(&txBytes, uint32_t{0});
}

void recordRx(const wifi_promiscuous_pkt_t* packet,
              wifi_promiscuous_pkt_type_t type) {
  if (packet == nullptr)
    return;

  __atomic_fetch_add(&rxPackets, uint32_t{1}, __ATOMIC_RELAXED);
  __atomic_fetch_add(&rxBytes, static_cast<uint32_t>(packet->rx_ctrl.sig_len),
                     __ATOMIC_RELAXED);
  switch (type) {
    case WIFI_PKT_MGMT:
      __atomic_fetch_add(&rxManagement, uint32_t{1}, __ATOMIC_RELAXED);
      break;
    case WIFI_PKT_CTRL:
      __atomic_fetch_add(&rxControl, uint32_t{1}, __ATOMIC_RELAXED);
      break;
    case WIFI_PKT_DATA:
      __atomic_fetch_add(&rxData, uint32_t{1}, __ATOMIC_RELAXED);
      break;
    default:
      __atomic_fetch_add(&rxMisc, uint32_t{1}, __ATOMIC_RELAXED);
      break;
  }

  const int8_t rssi = packet->rx_ctrl.rssi;
  __atomic_fetch_add(&rxRssiSum, static_cast<int32_t>(rssi),
                     __ATOMIC_RELAXED);
  storeRelaxed(&rxLastRssi, static_cast<int32_t>(rssi));
  updateStrongest(rssi);
  updateWeakest(rssi);
}

void recordTx(esp_err_t status, size_t bytes) {
  __atomic_fetch_add(&txAttempts, uint32_t{1}, __ATOMIC_RELAXED);
  if (status == ESP_OK) {
    __atomic_fetch_add(&txAccepted, uint32_t{1}, __ATOMIC_RELAXED);
    __atomic_fetch_add(&txBytes, static_cast<uint32_t>(bytes),
                       __ATOMIC_RELAXED);
  }
  else {
    __atomic_fetch_add(&txFailed, uint32_t{1}, __ATOMIC_RELAXED);
  }
}

void recordTxPower(int8_t requestedQdbm, esp_err_t setStatus,
                   int8_t effectiveQdbm, esp_err_t getStatus) {
  storeRelaxed(&requestedTxPowerQdbm, requestedQdbm);
  storeRelaxed(&effectiveTxPowerQdbm, effectiveQdbm);
  storeRelaxed(&txPowerSetStatus, static_cast<int32_t>(setStatus));
  storeRelaxed(&txPowerGetStatus, static_cast<int32_t>(getStatus));
  uint8_t flags = 0x01;
  if (getStatus == ESP_OK)
    flags |= 0x02;
  storeRelaxed(&txPowerFlags, flags);
}

void recordEffectiveTxPower(int8_t effectiveQdbm, esp_err_t getStatus) {
  storeRelaxed(&effectiveTxPowerQdbm, effectiveQdbm);
  storeRelaxed(&txPowerGetStatus, static_cast<int32_t>(getStatus));
  uint8_t flags = loadRelaxed(&txPowerFlags) & 0x01;
  if (getStatus == ESP_OK)
    flags |= 0x02;
  storeRelaxed(&txPowerFlags, flags);
}

Snapshot snapshot() {
  Snapshot result{};
  result.startedMs = loadRelaxed(&startedMs);
  result.rxPackets = loadRelaxed(&rxPackets);
  result.rxBytes = loadRelaxed(&rxBytes);
  result.rxManagement = loadRelaxed(&rxManagement);
  result.rxControl = loadRelaxed(&rxControl);
  result.rxData = loadRelaxed(&rxData);
  result.rxMisc = loadRelaxed(&rxMisc);
  result.rxRssiSum = loadRelaxed(&rxRssiSum);
  result.rxStrongestRssi = static_cast<int8_t>(loadRelaxed(&rxStrongestRssi));
  result.rxWeakestRssi = static_cast<int8_t>(loadRelaxed(&rxWeakestRssi));
  result.rxLastRssi = static_cast<int8_t>(loadRelaxed(&rxLastRssi));
  result.txAttempts = loadRelaxed(&txAttempts);
  result.txAccepted = loadRelaxed(&txAccepted);
  result.txFailed = loadRelaxed(&txFailed);
  result.txBytes = loadRelaxed(&txBytes);
  result.requestedTxPowerQdbm = loadRelaxed(&requestedTxPowerQdbm);
  result.effectiveTxPowerQdbm = loadRelaxed(&effectiveTxPowerQdbm);
  result.txPowerSetStatus =
      static_cast<esp_err_t>(loadRelaxed(&txPowerSetStatus));
  result.txPowerGetStatus =
      static_cast<esp_err_t>(loadRelaxed(&txPowerGetStatus));
  const uint8_t flags = loadRelaxed(&txPowerFlags);
  result.requestedTxPowerValid = (flags & 0x01) != 0;
  result.effectiveTxPowerValid = (flags & 0x02) != 0;
  return result;
}

}  // namespace RadioDiagnostics
