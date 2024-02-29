#ifndef COMMON_H
#define COMMON_H

#include "Parameters.h"


struct CRGB leds[MAX_NUM_LEDS];
struct CRGB previousLeds[MAX_NUM_LEDS];

CRGB blend(CRGB color1, int frac, CRGB color2) {
  // return (color1 * (1.0-multip)) + (color2 * multip);
  return color1.lerp8(color2, frac);
} // blend()

void applyTransition() {
  if (transStep > 0) {
    for(int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = blend(leds[i], transStep, previousLeds[i]);
    }
    transStep-=TR_SPEED;
  }
}

void initTransition() {
  transStep = 255;
  for(int i = 0; i < NUM_LEDS; ++i) {
      previousLeds[i] = leds[i];
  }
}

#endif // COMMON_H
