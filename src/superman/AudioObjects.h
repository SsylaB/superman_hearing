#pragma once
#include <Audio.h>

// Custom Noise Gate Class
class AudioEffectNoiseGate : public AudioStream {
  public:
    AudioEffectNoiseGate() : AudioStream(1, inputQueueArray) {}

    void setThreshold(int16_t t) {
      if (t < 0) t = 0;
      if (t > 32767) t = 32767;
      threshold = t;
    }
    void setClosedGain(float g) {
      if (g < 0.0f) g = 0.0f;
      if (g > 1.0f) g = 1.0f;
      closedGain = g;
    }
    void setAttackRelease(float a, float r) {
      if (a < 0.0f) a = 0.0f; if (a > 1.0f) a = 1.0f;
      if (r < 0.0f) r = 0.0f; if (r > 1.0f) r = 1.0f;
      attack = a;
      releaseCoeff = r;
    }

    virtual void update(void) override {
      audio_block_t *block = receiveReadOnly(0);
      if (!block) return;

      audio_block_t *out = allocate();
      if (!out) { AudioStream::release(block); return; }

      int16_t maxAbs = 0;
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int16_t s = block->data[i];
        int16_t a = (s < 0) ? (int16_t)(-s) : s;
        if (a > maxAbs) maxAbs = a;
      }

      float target = (maxAbs >= threshold) ? 1.0f : closedGain;
      float coeff  = (target > gain) ? attack : releaseCoeff;
      gain += (target - gain) * coeff;

      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int32_t v = (int32_t)(block->data[i] * gain);
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        out->data[i] = (int16_t)v;
      }

      transmit(out);
      AudioStream::release(out);
      AudioStream::release(block);
    }

  private:
    audio_block_t *inputQueueArray[1];
    int16_t threshold = 800;
    float closedGain  = 0.10f;
    float attack      = 0.20f;
    float releaseCoeff= 0.05f;
    float gain        = 1.0f;
};


// Use 'extern' so other files know these exist without creating duplicates
extern AudioInputI2S            lineIn;
extern AudioEffectDelay         delayL;
extern AudioEffectDelay         delayR;
extern AudioMixer4              fracMixL;
extern AudioMixer4              fracMixR;
extern AudioAmplifier           polR;
extern AudioMixer4              beamMixer;
extern AudioFilterBiquad        clarityHP;
extern AudioFilterBiquad        clarityLP;
extern AudioEffectNoiseGate     clarityGate;
extern AudioEffectFreeverb      fxReverb;
extern AudioMixer4              outSelect;
extern AudioControlSGTL5000     sgtl5000;
extern AudioAnalyzePeak         peakL;
extern AudioAnalyzePeak         peakR;