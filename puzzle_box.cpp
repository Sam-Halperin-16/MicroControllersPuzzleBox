/**
 * Puzzle Box program
 * Author: Micah Henney, Samuel Halperin, Noah Billman
 * 
 * Many simultaneous systems:
 * 1. IR input 4 digit code
 * 2. Matching puzzle based on ADC detecting 1.71129V +- 0.02V
 */
#include "state_machines/system.h"
#include "lib/ir_receiver.h"

 /**
  * State machines for each of the 4 puzzles:
  * 1. Tone match system
  */

// List of state machines to tick
static SystemStateMachine systemStateMachine;

int main() {

    systemStateMachine.setState(SystemState::Active);
    ir_receiver::init();
    // Main loop to tick all state machines
    while (true) {
        systemStateMachine.tick();
        ir_receiver::Event event;
        while (ir_receiver::poll(event)) {
            if (event.button == ir_receiver::Button::Digit) {
                systemStateMachine.onIrDigit(event.digit);
            } else if (event.button == ir_receiver::Button::Ok) {
                systemStateMachine.onIrOk();
            }
        }
    }

    // return 0;
}
