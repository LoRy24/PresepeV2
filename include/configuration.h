#ifndef PRESEPEV2_CONFIGURATION_H
#define PRESEPEV2_CONFIGURATION_H

#include <stdint.h>

namespace Configuration {
    constexpr uint8_t TriggerPin = 2;
    constexpr uint8_t EchoPin = 3;
    constexpr uint8_t BuzzerPin = 5;
    constexpr uint32_t SerialBaudRate = 9600;
    constexpr uint32_t SensorPollIntervalMs = 100;
    constexpr uint16_t SensorMaximumDistanceCm = 100;
    constexpr uint16_t ActivationDistanceCm = 50;
    constexpr uint16_t RearmDistanceCm = 65;
    constexpr uint8_t RequiredConsecutiveReadings = 2;
    constexpr uint8_t NoEchoReadingsToRearm = 5;

    constexpr uint16_t MusicTempoBpm = 168;
    constexpr uint16_t TuScendiTempoBpm = 120;
    constexpr uint8_t MusicArticulationPercent = 85;

    // Durate provvisorie per i componenti ancora da implementare.
    constexpr uint32_t CometDurationMs = 15000;
    constexpr uint32_t LightsDurationMs = 15000;

    static_assert(BuzzerPin != TriggerPin && BuzzerPin != EchoPin, "Il cicalino richiede un pin dedicato");
    static_assert(MusicTempoBpm > 0 && MusicTempoBpm <= 300, "Tempo musicale fuori intervallo");
    static_assert(TuScendiTempoBpm > 0 && TuScendiTempoBpm <= 300, "Tempo di Tu scendi dalle stelle fuori intervallo");
    static_assert(MusicArticulationPercent > 0 && MusicArticulationPercent < 100, "Serve uno stacco fra le note");
    static_assert(ActivationDistanceCm < RearmDistanceCm, "Le soglie devono avere isteresi");
    static_assert(RearmDistanceCm <= SensorMaximumDistanceCm, "Il sensore deve poter rilevare il riarmo");
    static_assert(RequiredConsecutiveReadings > 0, "Serve almeno una lettura di conferma");
    static_assert(NoEchoReadingsToRearm > 0, "L'assenza di eco deve essere confermata");
}

#endif //PRESEPEV2_CONFIGURATION_H
