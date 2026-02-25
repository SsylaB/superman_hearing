#include <Arduino.h>
#include "AudioLogic.h"

void updateSteeringDelays() {
    if (bypassMode) {
        delayL.delay(0, 0); delayR.delay(0, 0);
        fracMixL.gain(0, 1.0f); fracMixL.gain(1, 0.0f);
        fracMixR.gain(0, 1.0f); fracMixR.gain(1, 0.0f);
        return;
    }

    int rawPot = analogRead(POT_STEER);
    float angleRad = map(rawPot, 0, 1023, -90, 90) * (PI / 180.0f);
    float tau = (MIC_SPACING_M * sinf(angleRad)) / SPEED_OF_SOUND;
    float delaySamples = tau * (float)AUDIO_SAMPLE_RATE_EXACT;
    float delayAbs = fabsf(delaySamples);
    
    float Nf = floorf(delayAbs);
    float a  = delayAbs - Nf;

    // Helper to set hardware taps
    auto setTaps = [](AudioEffectDelay &dly, float samples) {
        dly.delay(0, (1000.0f * floorf(samples)) / AUDIO_SAMPLE_RATE_EXACT);
        dly.delay(1, (1000.0f * (floorf(samples) + 1.0f)) / AUDIO_SAMPLE_RATE_EXACT);
    };

    if (delaySamples >= 0.0f) {
        setTaps(delayL, delayAbs);
        delayR.delay(0, 0);
        fracMixL.gain(0, 1.0f - a); fracMixL.gain(1, a);
        fracMixR.gain(0, 1.0f);     fracMixR.gain(1, 0.0f);
    } else {
        setTaps(delayR, delayAbs);
        delayL.delay(0, 0);
        fracMixR.gain(0, 1.0f - a); fracMixR.gain(1, a);
        fracMixL.gain(0, 1.0f);     fracMixL.gain(1, 0.0f);
    }
}

void updateClarityParams(float clarity01) {
    if (clarity01 <= 0.001f) {
        clarityGate.setClosedGain(1.0f);
        return;
    }
    // Mapping logic from your original code
    float Q = 0.7f + (3.2f - 0.7f) * clarity01;
    clarityHP.setHighpass(0, 300.0f, Q);
    clarityLP.setLowpass(0, 3400.0f, Q);
    clarityGate.setThreshold((int16_t)(250 + (2600 - 250) * clarity01));
}

void updateFxParams(float fx01) {
    fxReverb.roomsize(0.15f + 0.40f * fx01);
    fxReverb.damping(0.30f + 0.55f * fx01);
}

void applyRouting() {
    if (bypassMode) {
        outSelect.gain(0, 0.0f); outSelect.gain(1, 0.0f); outSelect.gain(2, 1.0f);
    } else {
        outSelect.gain(2, 0.0f);
        outSelect.gain(0, g_mode == 0 ? 1.0f : 0.0f);
        outSelect.gain(1, g_mode == 1 ? 1.0f : 0.0f);
    }
}