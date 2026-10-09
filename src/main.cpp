#include <Arduino.h>
#include "ChristmasMusic.h"
#include "ProximityTrigger.h"
#include "ShowController.h"
#include "UltrasonicProximitySensor.h"
#include "configuration.h"

namespace {
    UltrasonicProximitySensor proximitySensor(Configuration::TriggerPin, Configuration::EchoPin);
    ProximityTrigger proximityTrigger;
    ShowController show;
    uint32_t lastSensorPollMs = 0;
}

void setup() {
    Serial.begin(Configuration::SerialBaudRate);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    proximitySensor.begin();
    proximitySensor.setMaximumDistanceToCheckInCm(Configuration::SensorMaximumDistanceCm);
    show.begin();
    lastSensorPollMs = millis();

    Serial.println(F("[Presepe] Pronto. Avvicinarsi per avviare lo spettacolo."));
}

void loop() {
    const uint32_t nowMs = millis();
    const bool wasRunning = show.isRunning();
    show.update(nowMs);

    if (wasRunning && !show.isRunning()) {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println(F("[Presepe] Spettacolo terminato. Allontanarsi per riarmare il sensore."));
    }

    // Il sensore usa pulseIn(): lo leggiamo soltanto a riproduzione inattiva,
    // così la breve attesa dell'eco non rallenta gli aggiornamenti dei componenti.
    if (show.isRunning() || static_cast<uint32_t>(nowMs - lastSensorPollMs) < Configuration::SensorPollIntervalMs) {
        return;
    }

    lastSensorPollMs = nowMs;
    const unsigned long distanceCm = proximitySensor.getDistanceInCm();
    if (proximityTrigger.update(distanceCm) && show.start(millis())) {
        digitalWrite(LED_BUILTIN, HIGH);
        if (ChristmasMusic::currentSong() == ChristmasMusic::Song::JingleBells) {
            Serial.println(F("[Presepe] Avvio: Jingle Bells, stella cometa e luci."));
        }
        else {
            Serial.println(F("[Presepe] Avvio: Tu scendi dalle stelle, stella cometa e luci."));
        }
    }
}
