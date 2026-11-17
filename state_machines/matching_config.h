#ifndef MATCHING_CONFIG_H
#define MATCHING_CONFIG_H

#include <cstdint>

namespace matching_config {
constexpr unsigned int MIN_HZ = 400;
constexpr unsigned int MAX_HZ = 1600;
constexpr unsigned int MATCH_TOLERANCE_HZ = 20;
constexpr unsigned int INITIAL_HZ = 1000;
constexpr int ENCODER_STEP_HZ = 10;
constexpr std::uint64_t TARGET_TONE_US = 5000000;
constexpr float ADC_REFERENCE_VOLTS = 3.3f;
constexpr std::uint16_t ADC_MAX = 4095;
constexpr float WIRE_TARGET_VOLTS = 1.71129f;
constexpr float WIRE_TOLERANCE_VOLTS = 0.02f;
constexpr std::uint64_t WIRE_SETTLE_US = 200000;
}

#endif
