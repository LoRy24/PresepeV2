#include "ShowController.h"
#include "AnimatedLights.h"
#include "ChristmasMusic.h"
#include "CometAnimation.h"

void ShowController::begin() {
    CometAnimation::begin();
    ChristmasMusic::begin();
    AnimatedLights::begin();
    running_ = false;
}

bool ShowController::start(uint32_t nowMs) {
    if (running_) {
        return false;
    }

    // Tutti i componenti condividono lo stesso istante iniziale.
    CometAnimation::start(nowMs);
    ChristmasMusic::start(nowMs);
    AnimatedLights::start(nowMs);
    running_ = true;
    return true;
}

void ShowController::update(uint32_t nowMs) {
    if (!running_) {
        return;
    }

    CometAnimation::update(nowMs);
    ChristmasMusic::update(nowMs);
    AnimatedLights::update(nowMs);

    // Basta un componente ancora attivo per impedire un nuovo avvio.
    running_ = CometAnimation::isRunning() || ChristmasMusic::isRunning() || AnimatedLights::isRunning();
}

void ShowController::stop() {
    CometAnimation::stop();
    ChristmasMusic::stop();
    AnimatedLights::stop();
    running_ = false;
}
