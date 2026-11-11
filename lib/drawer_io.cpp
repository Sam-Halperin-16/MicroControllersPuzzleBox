#include "drawer_io.h"

namespace drawer_io {
void setReleased(Drawer drawer, bool released) {
    // TODO: Configure each servo GPIO and its calibrated locked/released PWM
    // positions. Command the latch position without waiting for servo travel.
    // Released means commanded open; there are no drawer position sensors yet.
    (void)drawer;
    (void)released;
}

bool screwTouched(unsigned int screw) {
    // TODO: Read calibrated capacitive touch inputs, indexed 0 and 1.
    (void)screw;
    return false;
}

bool readPotentiometer(std::uint16_t& value) {
    // TODO: Configure the potentiometer ADC GPIO and return a 12-bit reading.
    (void)value;
    return false;
}

void playTone(unsigned int frequencyHz) {
    // TODO: Start speaker PWM on its configured GPIO. Do not block here.
    (void)frequencyHz;
}

void stopTone() {
    // TODO: Disable speaker PWM.
}
}
