#ifndef DRAWER_IO_H
#define DRAWER_IO_H

#include <cstdint>

// Hardware connection templates; fill in drawer_io.cpp for the installed parts.
namespace drawer_io {
enum class Drawer { Magnet, Wires };

void setReleased(Drawer drawer, bool released);
bool screwTouched(unsigned int screw);
// Return false when no valid sample is available; samples are 0..4095.
bool readPotentiometer(std::uint16_t& value);
void playTone(unsigned int frequencyHz);
void stopTone();
}

#endif // DRAWER_IO_H
