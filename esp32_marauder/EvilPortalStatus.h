#pragma once

#include <stdint.h>

enum class EvilPortalPhase : uint8_t {
  Waiting,
  Deauth,
  Client,
  Captive,
};

inline EvilPortalPhase evilPortalPhase(bool deauth_enabled,
                                       uint8_t connected_clients,
                                       bool portal_activity) {
  if (connected_clients > 0)
    return portal_activity ? EvilPortalPhase::Captive
                           : EvilPortalPhase::Client;
  return deauth_enabled ? EvilPortalPhase::Deauth
                        : EvilPortalPhase::Waiting;
}

inline const char* evilPortalPhaseLabel(EvilPortalPhase phase) {
  switch (phase) {
    case EvilPortalPhase::Deauth:
      return "DEAUTH";
    case EvilPortalPhase::Client:
      return "CLIENT";
    case EvilPortalPhase::Captive:
      return "CAPTIVE";
    case EvilPortalPhase::Waiting:
    default:
      return "WAITING";
  }
}
