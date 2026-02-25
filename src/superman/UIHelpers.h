#pragma once
#include "ProjectDefines.h"

void updateLEDs(bool bypass, int steerVal);
float potTo01Smoothed(int pin, int &filt, int weight);