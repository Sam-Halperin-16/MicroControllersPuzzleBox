#ifndef DRAWER_IO_H
#define DRAWER_IO_H

#include <cstdint>

// Main-loop hardware access for drawer actuators and puzzle peripherals.
namespace drawer_io {
enum class Drawer { Magnet, Wires };

// Command 140 degrees to release or 50 degrees to lock, without waiting for travel.
// Magnet servo: GP7; wire drawer servo: GP8. PWM continues to hold the position.
void setReleased(Drawer drawer, bool released);
bool screwTouched(unsigned int screw);
// Return false when no valid sample is available; samples are 0..4095.
bool readWireNetwork(std::uint16_t& value);
void setWireBacklight(bool enabled);
void playTone(unsigned int frequencyHz);
void stopTone();
}

#endif // DRAWER_IO_H
