#include "ProjectDefines.h"
#include "AudioObjects.h"
#include "UIHelpers.h"
#include "AudioLogic.h"

// Instantiate Global State
bool bypassMode = false;
int g_mode = 0;
int g_beamType = 0;
int s_clarityFilt, s_fxFilt;

// Instantiate Audio Objects (The actual creation)
AudioInputI2S lineIn;
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

// 1. Input to Delays (Pre-processing filters removed for simplicity as per your original logic)
AudioConnection patchCord1(lineIn, 0, delayL, 0);
AudioConnection patchCord2(lineIn, 1, delayR, 0);

// 2. Monitoring (Raw input to meters)
AudioConnection patchCord5(lineIn, 0, peakL, 0);
AudioConnection patchCord6(lineIn, 1, peakR, 0);

// 3. Delay Taps to Fractional Mixers
// Each delay line provides tap 0 (N) and tap 1 (N+1)
AudioConnection patchCord7(delayL, 0, fracMixL, 0);
AudioConnection patchCord8(delayL, 1, fracMixL, 1);
AudioConnection patchCord9(delayR, 0, fracMixR, 0);
AudioConnection patchCord10(delayR, 1, fracMixR, 1);

// 4. Beamforming Summation
// fracMixL -> beamMixer (Ch 0)
// fracMixR -> polR (for polarity) -> beamMixer (Ch 1)
AudioConnection patchCord11(fracMixR, 0, polR, 0);
AudioConnection patchCord12(fracMixL, 0, beamMixer, 0);
AudioConnection patchCord13(polR,    0, beamMixer, 1);

// 5. Clarity Chain (Mono beam output through filters and gate)
AudioConnection patchCord14(beamMixer, 0, clarityHP, 0);
AudioConnection patchCord15(clarityHP, 0, clarityLP, 0);
AudioConnection patchCord16(clarityLP, 0, clarityGate, 0);

// 6. FX Input Selection
// Mode 0/1 uses the clarityGate output
AudioConnection patchCord17(clarityGate, 0, fxInputMixer, 0);
// (Optional ch1 for raw input if you decide to expand later)
AudioConnection patchCord18(lineIn, 0, fxInputMixer, 1);

// 7. FX Chain
AudioConnection patchCord19(fxInputMixer, 0, fxReverb, 0);

// 8. Output Selector
// Ch0: Clarity Gate (Clean)
// Ch1: Reverb Output (Wet)
// Ch2: Direct Beam (Bypass)
AudioConnection patchCord20(clarityGate, 0, outSelect, 0);
AudioConnection patchCord21(fxReverb,    0, outSelect, 1);
AudioConnection patchCord22(beamMixer,   0, outSelect, 2);

// 9. Final Output (Mono to both Stereo channels)
AudioConnection patchCord23(outSelect, 0, i2s_out, 0);
AudioConnection patchCord24(outSelect, 0, i2s_out, 1);

void setup() {
    Serial.begin(115200);
    AudioMemory(60);
    sgtl5000.enable();
    sgtl5000.volume(0.6);
    
    pinMode(PIN_BYPASS_BTN, INPUT_PULLUP);
    pinMode(PIN_MODE_BTN, INPUT_PULLUP);
    // Initialize state
    applyRouting();
}

void loop() {
    // 1. Handle UI
    if (digitalRead(PIN_BYPASS_BTN) == LOW) {
        bypassMode = !bypassMode;
        applyRouting();
        delay(300); // Simple debounce
    }

    // 2. Update Audio
    float clarity = potTo01Smoothed(POT_CLARITY, s_clarityFilt, 16);
    float fx = potTo01Smoothed(POT_FX, s_fxFilt, 4);
    
    updateSteeringDelays();
    updateClarityParams(clarity);
    updateFxParams(fx);
    updateLEDs(bypassMode, analogRead(POT_STEER));

    // 3. Telemetry
    static elapsedMillis timer;
    if (timer > 50) {
        printLevelMeter();
        timer = 0;
    }
}