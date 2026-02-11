#include <Audio.h>
#include <Wire.h>

// 1. Audio Components
AudioInputI2S            lineIn;         
AudioMixer4              mixerL;         // Mixer for Left
AudioMixer4              mixerR;         // Mixer for Right
AudioOutputI2S           headphones;     

// 2. Patching (LineIn -> Mixer -> Headphones)
AudioConnection          patchCord1(lineIn, 0, mixerL, 0);
AudioConnection          patchCord2(lineIn, 1, mixerR, 0);
AudioConnection          patchCord3(mixerL, 0, headphones, 0);
AudioConnection          patchCord4(mixerR, 0, headphones, 1);

AudioControlSGTL5000     sgtl5000_1;

const int potVolPin = A0;
const int potGainPin = A1;
const int btnLPin = 0;
const int btnRPin = 1;

// State variables
bool muteL = false;
bool muteR = false;
int lastGain = -1;

void setup() {
  Serial.begin(9600);
  AudioMemory(15);
  
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.5);
  sgtl5000_1.unmuteHeadphone();

  // Initialize mixers to full volume (1.0)
  mixerL.gain(0, 1.0);
  mixerR.gain(0, 1.0);
  
  pinMode(btnLPin, INPUT_PULLUP);
  pinMode(btnRPin, INPUT_PULLUP);
}

void loop() {
  // --- Master Volume (Pot A0) ---
  float masterVol = analogRead(potVolPin) / 1023.0;
  sgtl5000_1.volume(masterVol);

  // --- Input Gain (Pot A1) ---
  int currentGain = map(analogRead(potGainPin), 0, 1023, 0, 15);
  if (currentGain != lastGain) {
    sgtl5000_1.lineInLevel(currentGain);
    lastGain = currentGain;
  }

  // --- Left Mute Toggle (Button 0) ---
  if (digitalRead(btnLPin) == LOW) {
    muteL = !muteL;
    mixerL.gain(0, muteL ? 0.0 : 1.0); // If muteL is true, gain is 0.0
    Serial.println(muteL ? "Left: MUTED" : "Left: ON");
    delay(300); 
  }

  // --- Right Mute Toggle (Button 1) ---
  if (digitalRead(btnRPin) == LOW) {
    muteR = !muteR;
    mixerR.gain(0, muteR ? 0.0 : 1.0); 
    Serial.println(muteR ? "Right: MUTED" : "Right: ON");
    delay(300);
  }
}