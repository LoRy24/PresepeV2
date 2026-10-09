#ifndef PRESEPEV2_COMET_ANIMATION_H
#define PRESEPEV2_COMET_ANIMATION_H

#include <stdint.h>

namespace CometAnimation {
    void begin();
    void start(uint32_t nowMs);
    void update(uint32_t nowMs);
    void stop();
    bool isRunning();
}

#endif
