#ifndef GPIO_INTERRUPTS_H
#define GPIO_INTERRUPTS_H

#include <cstdint>

// Shared owner of the Pico SDK GPIO callback. Register/consume on core 0 only.
// Drivers configure their own GPIO directions/pulls. Handlers run in interrupt
// context: capture inputs only; do not wait, print, or run puzzle logic here.
namespace gpio_interrupts {
using Handler = void (*)(unsigned int gpio, std::uint32_t events, void* context);

// One subscriber per pin, with a rising/falling edge mask. Returns false for
// invalid arguments, core 1, or a pin owned by another handler/context pair.
// Reattaching the same pair updates its mask without disturbing other pins.
bool attach(unsigned int gpio, std::uint32_t edges, Handler handler, void* context = nullptr);
bool detach(unsigned int gpio, Handler handler, void* context = nullptr);
}

#endif
