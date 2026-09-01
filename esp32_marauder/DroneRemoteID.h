#ifndef DRONE_REMOTE_ID_H
#define DRONE_REMOTE_ID_H

#include <stdint.h>

namespace DroneRemoteID {

// Passively receives and decodes ASTM/OpenDroneID broadcasts carried in BLE
// service data, Wi-Fi Beacon vendor IEs, and Wi-Fi NAN service descriptors.
void run();

}  // namespace DroneRemoteID

#endif
