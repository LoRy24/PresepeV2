#ifndef PRESEPEV2_TEST_ARDUINO_H
#define PRESEPEV2_TEST_ARDUINO_H

#include <stdint.h>

constexpr uint8_t OUTPUT = 1;
constexpr uint8_t LOW = 0;

namespace FakeArduino {
    struct BuzzerState {
        uint8_t configuredPin = 0;
        uint8_t mode = 0;
        uint8_t tonePin = 0;
        uint16_t frequencyHz = 0;
        unsigned long durationMs = 0;
        unsigned int toneCalls = 0;
        unsigned int silenceCalls = 0;
        uint8_t writtenPin = 0;
        uint8_t writtenValue = 0;
        bool sounding = false;
    };

    inline BuzzerState& state() {
        static BuzzerState buzzer;
        return buzzer;
    }

    inline void reset() { state() = BuzzerState(); }
}

inline void pinMode(uint8_t pin, uint8_t mode) {
    FakeArduino::state().configuredPin = pin;
    FakeArduino::state().mode = mode;
}

inline void digitalWrite(uint8_t pin, uint8_t value) {
    FakeArduino::state().writtenPin = pin;
    FakeArduino::state().writtenValue = value;
}

inline void tone(uint8_t pin, unsigned int frequency, unsigned long duration = 0) {
    FakeArduino::state().tonePin = pin;
    FakeArduino::state().frequencyHz = frequency;
    FakeArduino::state().durationMs = duration;
    ++FakeArduino::state().toneCalls;
    FakeArduino::state().sounding = true;
}

inline void noTone(uint8_t pin) {
    FakeArduino::state().tonePin = pin;
    ++FakeArduino::state().silenceCalls;
    FakeArduino::state().sounding = false;
}

#endif
