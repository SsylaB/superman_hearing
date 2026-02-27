#include "ProjectDefines.h"
#include "AudioObjects.h"
#include "UIHelpers.h"
#include "AudioLogic.h"

// Instantiate Global State
bool bypassMode = false;
int g_mode = 0;
int g_beamType = 0;
int s_clarityFilt, s_volumeFilt;
elapsedMillis fps; // Chronomètre global pour l'affichage

// Instantiate Audio Objects
AudioInputI2S lineIn;
AudioOutputI2S i2s_out;      
AudioMixer4 fxInputMixer;
AudioEffectDelay delayL, delayR;
AudioMixer4 fracMixL, fracMixR, beamMixer, outSelect;
AudioAmplifier polR;
AudioFilterBiquad clarityHP, clarityLP;
AudioEffectNoiseGate clarityGate;
AudioEffectFreeverb fxReverb;
AudioControlSGTL5000 sgtl5000;
AudioAnalyzePeak peakL, peakR;

// ======================================================
// AUDIO CONNECTIONS (The Patch Bay)
// ======================================================

AudioConnection patchCord1(lineIn, 0, delayL, 0);
AudioConnection patchCord2(lineIn, 1, delayR, 0);
AudioConnection patchCord5(lineIn, 0, peakL, 0);
AudioConnection patchCord6(lineIn, 1, peakR, 0);
AudioConnection patchCord7(delayL, 0, fracMixL, 0);
AudioConnection patchCord8(delayL, 1, fracMixL, 1);
AudioConnection patchCord9(delayR, 0, fracMixR, 0);
AudioConnection patchCord10(delayR, 1, fracMixR, 1);
AudioConnection patchCord11(fracMixR, 0, polR, 0);
AudioConnection patchCord12(fracMixL, 0, beamMixer, 0);
AudioConnection patchCord13(polR,    0, beamMixer, 1);
AudioConnection patchCord14(beamMixer, 0, clarityHP, 0);
AudioConnection patchCord15(clarityHP, 0, clarityLP, 0);
AudioConnection patchCord16(clarityLP, 0, clarityGate, 0);
AudioConnection patchCord17(clarityGate, 0, fxInputMixer, 0);
AudioConnection patchCord18(lineIn, 0, fxInputMixer, 1);
AudioConnection patchCord19(fxInputMixer, 0, fxReverb, 0);
AudioConnection patchCord20(clarityGate, 0, outSelect, 0);
AudioConnection patchCord21(fxReverb,    0, outSelect, 1);
AudioConnection patchCord22(beamMixer,   0, outSelect, 2);
AudioConnection patchCord23(outSelect, 0, i2s_out, 0);
AudioConnection patchCord24(outSelect, 0, i2s_out, 1);

void setup() {
    Serial.begin(115200);
    AudioMemory(60);
    sgtl5000.enable();
    
    // Configuration des boutons
    pinMode(PIN_BYPASS_BTN, INPUT_PULLUP);
    pinMode(PIN_MODE_BTN, INPUT_PULLUP);
    
    // CONFIGURATION DES LED (Indispensable pour l'affichage physique)
    pinMode(LED_BYPASS, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);
    
    // Initialisation des filtres des potentiomètres
    s_clarityFilt = analogRead(POT_CLARITY);
    s_volumeFilt = analogRead(POT_Volume);

    applyRouting(); // Applique le routage initial
}

void loop() {
    // 1. Gestion du bouton Bypass
    if (digitalRead(PIN_BYPASS_BTN) == LOW) {
        bypassMode = !bypassMode;
        applyRouting();
        updateLEDs(bypassMode, analogRead(POT_STEER));
        delay(300); // Anti-rebond
    }

    // 2. Gestion du bouton Mode (Correction : manquait dans loop)
    if (digitalRead(PIN_MODE_BTN) == LOW) {
        g_mode = (g_mode + 1) % 2; 
        applyRouting();
        Serial.print(">>> MODE: ");
        Serial.println(g_mode == 0 ? "CLARITY ONLY" : "CLARITY + REVERB");
        delay(300); // Anti-rebond
    }

    // 3. Mise à jour Audio
    float clarity = potTo01Smoothed(POT_CLARITY, s_clarityFilt, 16);
    float volume = potTo01Smoothed(POT_Volume, s_volumeFilt, 4);
    sgtl5000.volume(volume); // Le Pot 3 contrôle le volume général
    
    updateSteeringDelays();
    updateClarityParams(clarity);
    
    // Mise à jour des LED selon la direction
    updateLEDs(bypassMode, analogRead(POT_STEER));

    // 4. Télémesure (Affichage terminal)
    if (fps > 50) {
        printLevelMeter();
        fps = 0;
    }
}

// ======================================================
// ASCII meter - Définition globale (sans 'static')
// ======================================================
void printLevelMeter() {
  if (peakL.available() && peakR.available()) {
    float left = peakL.read();
    float right = peakR.read();

    Serial.println("\n============================================================");
    Serial.print("  SYSTEM: ");
    Serial.print(bypassMode ? "[!] BYPASS ACTIVE" : "[v] PROCESSING ACTIVE");
    Serial.print(" | MODE: ");
    Serial.print(g_mode == 0 ? "0 (Clarity Only)" : "1 (Clarity + REVERB)");
    Serial.print(" | BEAM: ");
    Serial.println(g_beamType == 0 ? "SUM" : "DIFF");
    Serial.println("------------------------------------------------------------");

    if (!bypassMode) {
      int rawPot = analogRead(POT_STEER);
      int steerPos = map(rawPot, 0, 1023, 0, 60);
      Serial.print("STEER: ");
      for (int i=0; i<60; i++) {
        if (i == 30) Serial.print("|");
        else if (i == steerPos) Serial.print("^");
        else Serial.print("-");
      }
      Serial.println();
    }

    Serial.print("L: ");
    int leftBars = left * 30;
    for (int i=0; i<30; i++) Serial.print(i < leftBars ? "|" : " ");
    
    Serial.print("  R: ");
    int rightBars = right * 30;
    for (int i=0; i<30; i++) Serial.print(i < rightBars ? "|" : " ");
    Serial.println("\n============================================================");
  }
}