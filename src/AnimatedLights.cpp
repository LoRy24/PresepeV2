#include "AnimatedLights.h"
#include "ActivityTimer.h"
#include "configuration.h"

namespace {
    ActivityTimer activity;
}

namespace AnimatedLights {
    void begin() {
        activity.stop();
        // TODO: inizializzare le uscite o la striscia LED scelta.
    }

    void start(uint32_t nowMs) {
        activity.start(nowMs, Configuration::LightsDurationMs);
        // TODO: impostare il primo fotogramma delle luci.
    }

    void update(uint32_t nowMs) {
        if (!activity.isRunning()) {
            return;
        }

        // TODO: aggiornare un fotogramma quando scade il relativo intervallo.
        activity.update(nowMs);
        if (!activity.isRunning()) {
            stop();
        }
    }

    void stop() {
        activity.stop();
        // TODO: spegnere le luci o ripristinare la scena di riposo.
    }

    bool isRunning() { return activity.isRunning(); }
}
