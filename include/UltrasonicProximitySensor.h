/*
 * \brief Get distance by using an ultrasonic proximity sensor
 *
 * \note Maximum distance that can be found using getDistanceInCm() is 20 meters.
 * This value can be increased or reduced by calling \p setMaximumDistanceToCheckInCm().
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 3 July 2016
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2016 Quentin Comte-Gaz
 * \version 1.0
 *
 * SYNTAX REVISION: Lorenzo Rocca
 */

#ifndef UltrasonicProximitySensor_h
#define UltrasonicProximitySensor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#define DEFAULT_MAX_DISTANCE_IN_CM 2000 /*!< Default maximum distance to check with sensor: 20 meters */

class UltrasonicProximitySensor {
    public:
        /*!
         * \brief UltrasonicProximitySensor Initialize Ultrasonic proximity sensor
         *
         * \param pin_trigger (int) Digital pin connected to Trigger sensor pin
         * \param pin_echo (int) Digital pin connected to Echo sensor pin
         */
        UltrasonicProximitySensor(int pin_trigger, int pin_echo);

        // Configurare i pin in setup(), dopo l'inizializzazione del runtime Arduino.
        void begin();

        /*!
         * \brief setMaximumDistanceToCheckInCm Set a maximum distance to prevent getting false positive values
         *
         * Default value: \p DEFAULT_MAX_DISTANCE_IN_CM
         *
         * \param max_distance_in_cm (unsigned long) Maximum distance the sensor should try to get.
         *        Smaller this value is, faster getDistance() will be if distance is too high.
         *
         * \return (bool) Valid input value (> 0)
         */
        bool setMaximumDistanceToCheckInCm(unsigned long max_distance_in_cm);

        /*!
         * \brief getMaximumDistanceToCheckInCm Get the maximum distance defined for the sensor
         *
         * Default value: \p DEFAULT_MAX_DISTANCE_IN_CM
         *
         * \return (unsigned long) Maximum distance the sensor should try to get.
         */
        unsigned long getMaximumDistanceToCheckInCm() const;

        /*!
         * \brief getDistanceInCm Get distance between sensor and any element
         *
         * \return (unsigned long) Distance between the sensor and any element (in cm)
         */
        unsigned long getDistanceInCm() const;




    private:
        static unsigned long usToCm(unsigned long time_in_us); /*!< microSec to Cm conversion */
        static unsigned long cmToUs(unsigned long distance_in_cm); /*!< Cm to microSec conversion */
        void sendTrigger() const; /*!< Send a trigger to the sensor */
        unsigned long waitEcho() const; /*!< Wait an echo from the sensor */

        int _pin_trigger; /*!< Digital Pin used as trigger */
        int _pin_echo; /*!< Digital Pin used as echo */
        unsigned long _max_distance_in_cm{}; /*!< Maximum distance (in cm) the sensor should try to get */
        unsigned long _timeout{}; /*!< Max time to wait echo after a trigger (linked to \p _max_distance_in_cm) */
};

#endif //UltrasonicProximitySensor_h
