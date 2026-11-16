#ifndef DRAWER_IO_H
#define DRAWER_IO_H

#include <cstdint>

// Main-loop hardware access. Drawer actuators and touch inputs remain templates.
namespace drawer_io {
enum class Drawer { Magnet, Wires };

void setReleased(Drawer drawer, bool released);
bool screwTouched(unsigned int screw);
// Return false when no valid sample is available; samples are 0..4095.
bool readPotentiometer(std::uint16_t& value);
bool readWireNetwork(std::uint16_t& value);
void setWireBacklight(bool enabled);
void playTone(unsigned int frequencyHz);
void stopTone();
}

#endif // DRAWER_IO_H
