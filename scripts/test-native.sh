#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
TEST_DIR="$(mktemp -d /tmp/marauder-eternal-native-tests.XXXXXX)"
trap 'rm -rf -- "${TEST_DIR}"' EXIT

g++ -std=c++17 -Wall -Wextra -Werror \
  -I"${PROJECT_DIR}/esp32_marauder" \
  "${PROJECT_DIR}/tests/native/test_helpers.cpp" \
  "${PROJECT_DIR}/esp32_marauder/BeaconFrame.cpp" \
  "${PROJECT_DIR}/esp32_marauder/DeauthFrame.cpp" \
  "${PROJECT_DIR}/esp32_marauder/DisplayLine.cpp" \
  "${PROJECT_DIR}/esp32_marauder/RsnCapabilities.cpp" \
  "${PROJECT_DIR}/esp32_marauder/SdTransferPath.cpp" \
  "${PROJECT_DIR}/esp32_marauder/UtcTime.cpp" \
  "${PROJECT_DIR}/esp32_marauder/WdgResponse.cpp" \
  -o "${TEST_DIR}/test_helpers"

"${TEST_DIR}/test_helpers"
echo "Native helper tests passed"
