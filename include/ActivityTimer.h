#ifndef PRESEPEV2_ACTIVITY_TIMER_H
#define PRESEPEV2_ACTIVITY_TIMER_H

#include <stdint.h>

// Stato comune ai segnaposto dei componenti, senza allocazioni dinamiche.
class ActivityTimer {
public:
    void start(uint32_t nowMs, uint32_t durationMs) {
        startedAtMs_ = nowMs;
        durationMs_ = durationMs;
        running_ = durationMs > 0;
    }

    void update(uint32_t nowMs) {
        // La sottrazione unsigned gestisce anche il rollover di millis().
        if (running_ && static_cast<uint32_t>(nowMs - startedAtMs_) >= durationMs_) {
            stop();
        }
    }

    void stop() { running_ = false; }
    bool isRunning() const { return running_; }

private:
    uint32_t startedAtMs_ = 0;
    uint32_t durationMs_ = 0;
    bool running_ = false;
};

#endif
