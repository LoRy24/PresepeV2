#include "CometAnimation.h"
#include "ActivityTimer.h"
#include "configuration.h"

namespace {
    ActivityTimer activity;
}

namespace CometAnimation {
    void begin() {
        activity.stop();
        // TODO: inizializzare il servo e la posizione di riposo.
    }

    void start(uint32_t nowMs) {
        activity.start(nowMs, Configuration::CometDurationMs);
        // TODO: impostare il primo passo dell'animazione.
    }

    void update(uint32_t nowMs) {
        if (!activity.isRunning()) {
            return;
        }

        // TODO: avanzare il servo di un passo quando scade il relativo intervallo.
        activity.update(nowMs);
        if (!activity.isRunning()) {
            stop();
        }
    }

    void stop() {
        activity.stop();
        // TODO: completare il ritorno alla posizione di riposo prima di dichiarare la fine.
    }

    bool isRunning() { return activity.isRunning(); }
}
