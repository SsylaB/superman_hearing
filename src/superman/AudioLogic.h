#pragma once
#include <Arduino.h>
#include "AudioObjects.h"
#include "ProjectDefines.h"

// External state shared with the main sketch
extern bool bypassMode;
extern int g_mode;
extern int g_beamType;

void updateSteeringDelays();
void updateClarityParams(float clarity01);
void updateFxParams(float fx01);
void applyRouting();
void printLevelMeter();