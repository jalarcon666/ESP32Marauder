#include <unity.h>

#include "EvilPortalStatus.h"

void setUp() {}
void tearDown() {}

void test_waiting_without_deauth_or_client() {
  TEST_ASSERT_EQUAL_UINT8(
      static_cast<uint8_t>(EvilPortalPhase::Waiting),
      static_cast<uint8_t>(evilPortalPhase(false, 0, false)));
}

void test_deauth_while_searching_for_client() {
  TEST_ASSERT_EQUAL_UINT8(
      static_cast<uint8_t>(EvilPortalPhase::Deauth),
      static_cast<uint8_t>(evilPortalPhase(true, 0, false)));
}

void test_client_pauses_deauth_before_http_probe() {
  TEST_ASSERT_EQUAL_UINT8(
      static_cast<uint8_t>(EvilPortalPhase::Client),
      static_cast<uint8_t>(evilPortalPhase(true, 1, false)));
}

void test_captive_after_client_reaches_http_portal() {
  TEST_ASSERT_EQUAL_UINT8(
      static_cast<uint8_t>(EvilPortalPhase::Captive),
      static_cast<uint8_t>(evilPortalPhase(true, 1, true)));
  TEST_ASSERT_EQUAL_STRING(
      "CAPTIVE", evilPortalPhaseLabel(EvilPortalPhase::Captive));
}

int main(int argc, char** argv) {
  UNITY_BEGIN();
  RUN_TEST(test_waiting_without_deauth_or_client);
  RUN_TEST(test_deauth_while_searching_for_client);
  RUN_TEST(test_client_pauses_deauth_before_http_probe);
  RUN_TEST(test_captive_after_client_reaches_http_portal);
  return UNITY_END();
}
