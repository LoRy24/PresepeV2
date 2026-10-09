#ifndef PRESEPEV2_SHOW_CONTROLLER_H
#define PRESEPEV2_SHOW_CONTROLLER_H

#include <stdint.h>

// Un solo coordinatore per i tre moduli, che rappresentano un unico spettacolo.
class ShowController {
public:
    void begin();
    bool start(uint32_t nowMs);
    void update(uint32_t nowMs);
    void stop();
    bool isRunning() const { return running_; }

private:
    bool running_ = false;
};

#endif
