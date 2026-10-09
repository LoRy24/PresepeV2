#include <Arduino.h>
#include <UltrasonicProximitySensor.h>
#include <configuration.h>

static UltrasonicProximitySensor proximitySensor(TRIGGER_PIN, ECHO_PIN);

void setup() {
    Serial.begin(9600);
    pinMode(13, OUTPUT);
    proximitySensor.setMaximumDistanceToCheckInCm(100);
}

void loop() {
    unsigned long distance = proximitySensor.getDistanceInCm();

    if (distance > 0 && distance < 50) {
        Serial.println("[ALERT] Ti sei avvicinato troppo! " + String(distance) + "cm");
        digitalWrite(13, HIGH);
    }
    else if (distance >= 50 && distance < 100) {
        Serial.println("[ALERT] Stammi alla larga! " + String(distance) + "cm");
    }

    delay(250);
    digitalWrite(13, LOW);
}