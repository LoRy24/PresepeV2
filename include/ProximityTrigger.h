#ifndef PRESEPEV2_PROXIMITY_TRIGGER_H
#define PRESEPEV2_PROXIMITY_TRIGGER_H

#include <stdint.h>

class ProximityTrigger {
public:
    // Chiamare una sola volta per ogni nuova misura, solo a spettacolo inattivo.
    // Distanza zero = nessun eco: mai un avvio, riarmo solo dopo assenza prolungata.
    bool update(uint32_t distanceCm);

private:
    bool armed_ = true;
    uint8_t consecutiveReadings_ = 0;
    uint8_t noEchoReadings_ = 0;
};

#endif
