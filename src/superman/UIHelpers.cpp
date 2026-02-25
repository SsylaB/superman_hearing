#include <Arduino.h>
#include "UIHelpers.h"

void updateLEDs(bool bypass, int steerVal) {
    digitalWrite(LED_BYPASS, bypass ? HIGH : LOW);
    if (bypass) {
        digitalWrite(PIN_LED2, LOW);
        digitalWrite(PIN_LED3, LOW);
    } else {
        if (steerVal < 450) { digitalWrite(PIN_LED3, HIGH); digitalWrite(PIN_LED2, LOW); }
        else if (steerVal > 570) { digitalWrite(PIN_LED3, LOW); digitalWrite(PIN_LED2, HIGH); }
        else { digitalWrite(PIN_LED2, HIGH); digitalWrite(PIN_LED3, HIGH); }
    }
}

float potTo01Smoothed(int pin, int &filt, int weight) {
    int raw = analogRead(pin);
    filt = (filt * (weight - 1) + raw) / weight;
    return (float)filt / 1023.0f;
}