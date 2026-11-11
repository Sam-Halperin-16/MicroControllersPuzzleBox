# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(puzzle_box C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

# Compile the application AND the independent display driver.
# Header files are included by the source files.
add_executable(puzzle_box
    globals.cpp
    lib/gpio_expander.cpp
    lib/seven_segment.cpp
    lib/ir_receiver.cpp
    lib/drawer_io.cpp
    puzzle_box.cpp
    # accelerometer.cpp
    state_machines/morse_leds.cpp
    state_machines/shake_vibrate.cpp
    state_machines/system.cpp
    state_machines/screen.cpp
    state_machines/drawers.cpp
)

pico_set_program_name(puzzle_box "puzzle_box")
pico_set_program_version(puzzle_box "0.1")

# Provides GPIO, timing, and standard Pico functionality.
# No hardware_i2c library is needed for this TM1637 driver.
target_link_libraries(puzzle_box
    pico_stdlib
    pico_rand
)

# Send diagnostic messages over USB serial.
pico_enable_stdio_usb(puzzle_box 1)
pico_enable_stdio_uart(puzzle_box 0)

# Find headers such as seven_segment.h in the project directory.
target_include_directories(puzzle_box PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Generate firmware outputs, including puzzle_box.uf2.
pico_add_extra_outputs(puzzle_box)
