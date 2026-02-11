#include <Audio.h>
#include "beepboop.h"  // Your generated header

beepboop mydsp;  // Faust instance
AudioOutputI2S out;  // Stereo I2S for shield
AudioControlSGTL5000 shield;  // Audio shield control

AudioConnection patchCordL(mydsp, 0, out, 0);
AudioConnection patchCordR(mydsp, 0, out, 1);  // Mono sine to both channels

void setup() {
  Serial.begin(115200); delay(500); Serial.println("Starting...");
  
  AudioMemory(12);  // Increase if needed
  
  if (!shield.enable()) { Serial.println("Shield init FAILED!"); while(1); }
  Serial.println("Shield OK");
  shield.volume(0.5);  // 0.3-0.7 typical
  
  // FIXED: Set params in setup() ONLY - loop kills audio!
  mydsp.setParamValue("freq", 440.0f);
  mydsp.setParamValue("gate", 1.0f);  // ON by default
  Serial.println("Faust params set");
}
void loop() {
  static unsigned long lastUpdate = 0;
  static int lastPot = -1;
  static int lastBtn = -1;

  int potRaw = analogRead(14);  // 0-1023
  float freq = 200.0f + (potRaw * 1800.0f / 1023.0f);  // Map to 200-2000 Hz

  int btnState = !digitalRead(0);  // Pressed=1 (pullup)

  // Serial debug (throttled)
  if (abs(potRaw - lastPot) > 5 || btnState != lastBtn) {
    Serial.print("Pot raw: "); Serial.print(potRaw);
    Serial.print(" | Freq: "); Serial.print(freq, 1);
    Serial.print(" | Gate: "); Serial.println(btnState);
    lastPot = potRaw;
    lastBtn = btnState;
  }

  // Update Faust params slowly to avoid blocking audio (Teensy 4.x safe rate)
  if (millis() - lastUpdate > 30) {  // ~33 Hz update, smooth enough
    mydsp.setParamValue("freq", freq);
    mydsp.setParamValue("gate", (float)btnState);
    lastUpdate = millis();
  }

  delayMicroseconds(500);  // Light CPU load
}
