#pragma once
#include <Arduino.h>

// PINS
const int PIN_BYPASS_BTN = 5;
const int PIN_MODE_BTN   = 2;
const int POT_STEER      = A1;
const int POT_CLARITY    = A0;
const int POT_FX         = A2;
const int LED_BYPASS     = 13;
const int PIN_LED2       = 3;
const int PIN_LED3       = 4;

// TUNING
const float CLARITY_HP_HZ = 300.0f;
const float CLARITY_LP_HZ = 3400.0f;
const float CLARITY_Q_MIN = 0.7f;
const float CLARITY_Q_MAX = 3.2f;

// BEAMFORMING
const float MIC_SPACING_M = 0.15f;
const float SPEED_OF_SOUND = 343.0f;
const uint32_t LONG_PRESS_MS = 900;