#include "ProximityTrigger.h"
#include "configuration.h"

bool ProximityTrigger::update(uint32_t distanceCm) {
    if (distanceCm == 0) {
        consecutiveReadings_ = 0;
        if (!armed_ && ++noEchoReadings_ >= Configuration::NoEchoReadingsToRearm) {
            armed_ = true;
            noEchoReadings_ = 0;
        }
        return false;
    }

    noEchoReadings_ = 0;
    const bool confirmsTransition = distanceCm > 0 && distanceCm <= Configuration::SensorMaximumDistanceCm &&
                                    (armed_ ? distanceCm <= Configuration::ActivationDistanceCm
                                            : distanceCm >= Configuration::RearmDistanceCm);

    if (!confirmsTransition) {
        consecutiveReadings_ = 0;
        return false;
    }

    if (++consecutiveReadings_ < Configuration::RequiredConsecutiveReadings) {
        return false;
    }

    consecutiveReadings_ = 0;
    const bool shouldStart = armed_;
    armed_ = !armed_;
    return shouldStart;
}
