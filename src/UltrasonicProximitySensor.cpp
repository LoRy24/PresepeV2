/*
 * \brief Get distance by using an ultrasonic proximity sensor (implementation)
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 3 July 2016
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2016 Quentin Comte-Gaz
 * \version 1.0
 *
 * SYNTAX REVISION: Lorenzo Rocca
 */

#include "UltrasonicProximitySensor.h"

UltrasonicProximitySensor::UltrasonicProximitySensor(int pin_trigger, int pin_echo) {
    _pin_trigger = pin_trigger;
    _pin_echo = pin_echo;

    pinMode(_pin_trigger, OUTPUT);
    pinMode(_pin_echo, INPUT);

    setMaximumDistanceToCheckInCm(DEFAULT_MAX_DISTANCE_IN_CM);
}

bool UltrasonicProximitySensor::setMaximumDistanceToCheckInCm(unsigned long max_distance_in_cm) {
    if (max_distance_in_cm <= 0) {
        return false;
    }

    _max_distance_in_cm = max_distance_in_cm;

    // Because of pulsIn() implementation, it is needed to add a fixed "12cm" to
    // the calibration. This number has been found as the real slice between
    // theory and reality with 3 different sensors. Indeed, pulsIn() timeout
    // is designed with this a time slice.
    _timeout = cmToUs(_max_distance_in_cm + 12);

    return true;
}

unsigned long UltrasonicProximitySensor::getMaximumDistanceToCheckInCm() const {
    return _max_distance_in_cm;
}

unsigned long UltrasonicProximitySensor::getDistanceInCm() const {
    unsigned long ditance_in_cm = 0;
    sendTrigger();
    ditance_in_cm = waitEcho();

    if (ditance_in_cm <= 0 || ditance_in_cm > _max_distance_in_cm) {
        return 0;
    }

    return ditance_in_cm;
}

////////////////////////////////PRIVATE FUNCTIONS//////////////////////////////

// Sensor calculus:
// Note: distance is twice the one we want to measure: Sensor -> Wall -> Sensor
//
// v = (2*d) / t
// with: v = 340 [m/s] = 0.034 [cm/us] (= 34/1000 [cm/us])
//
// --> t [us] = 2 * d [cm] / (34/1000) [cm/us]
// --> t [us] = 58 * d [cm] and d [cm] = t [us] / 58

unsigned long UltrasonicProximitySensor::usToCm(unsigned long time_in_us) {
    return time_in_us / 58;
}

unsigned long UltrasonicProximitySensor::cmToUs(unsigned long distance_in_cm) {
    return 58 * distance_in_cm;
}

void UltrasonicProximitySensor::sendTrigger() const {
    digitalWrite(_pin_trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(_pin_trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(_pin_trigger, LOW);
    delayMicroseconds(2);
}

unsigned long UltrasonicProximitySensor::waitEcho() const {
    unsigned long time_echo = pulseIn(_pin_echo, HIGH, _timeout);
    if (time_echo <= 0) {
      return 0;
    }

    return usToCm(time_echo);
}
