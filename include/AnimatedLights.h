#ifndef PRESEPEV2_ANIMATED_LIGHTS_H
#define PRESEPEV2_ANIMATED_LIGHTS_H

#include <stdint.h>

namespace AnimatedLights {
    void begin();
    void start(uint32_t nowMs);
    void update(uint32_t nowMs);
    void stop();
    bool isRunning();
}

#endif
