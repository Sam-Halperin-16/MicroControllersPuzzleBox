#include <cstdint>
#include <cstdio>
#include "pico/stdlib.h"
#include "seven_segment.h"
#include "accelerometer.h"

namespace {
// Central application pin assignments; reserve these for the display.
// CLK -> GP14 (physical pin 19), DIO -> GP15 (physical pin 20).
// PWR -> 3V3(OUT) (physical pin 36), GND -> GND (physical pin 18).
constexpr unsigned int DISPLAY_CLK = 14;
constexpr unsigned int DISPLAY_DIO = 15;
constexpr int DISPLAY_BRIGHTNESS = 3;
constexpr uint64_t COUNT_INTERVAL_US = 1000000;  // One count per second.

bool show_count(unsigned int count) {
    // Split application data into the four separate digits the driver needs.
    return seven_segment::set_number(
        (count / 1000) % 10,  // Thousands: leftmost digit.
        (count / 100) % 10,   // Hundreds.
        (count / 10) % 10,    // Tens.
        count % 10           // Ones: rightmost digit.
    );
}
}  // namespace

int main() {
    stdio_init_all();  // USB serial diagnostics; does not wait for a computer.

    if (!seven_segment::init(DISPLAY_CLK, DISPLAY_DIO, DISPLAY_BRIGHTNESS)) {
        // Stop startup on failure. Check the wiring/power, then reset Pico.
        // This short repeated message remains visible if USB connects later.
        while (true) {
            std::printf("Display init failed: check CLK, DIO and 3.3 V power.\n");
            sleep_ms(1000);
        }
    }
    // Later: initialize additional component drivers here.

    unsigned int count = 0;  // init() already displayed 0000.
    uint64_t next_count_at = time_us_64() + COUNT_INTERVAL_US;

    while (true) {
        const uint64_t now = time_us_64();
        if (now >= next_count_at) {
            // Catch up if another component briefly delays the loop. Base
            // the next deadline on the old one to avoid cumulative drift.
            const uint64_t intervals =
                (now - next_count_at) / COUNT_INTERVAL_US + 1;
            count = static_cast<unsigned int>((count + intervals) % 10000);
            next_count_at += intervals * COUNT_INTERVAL_US;

            if (!show_count(count)) {
                // Keep the application running; the next second retries a
                // fresh update rather than blocking all future components.
                std::printf("Display update failed: missing acknowledgement.\n");
            }
        }

        // Later: poll each independent component here on every loop pass.
        // Use short updates, rather than waiting for an entire puzzle action.
        // The display bus transfer is synchronous and takes a few ms.
        sleep_ms(10);  // Short idle pause, NOT a one-second counter delay.
    }
}
