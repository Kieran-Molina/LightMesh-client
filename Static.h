#ifndef STATIC_H
#define STATIC_H

#include "Common.h"

bool transFlag = false;

void initStatic() {
  //Serial.println("Static init");
  CRGB color = (currentEffect == EFFECT_OFF || !onoff) ? CRGB(0x000000) : COLOR_MAIN;
  for (int i = 0; i<NUM_LEDS; ++i) {
    leds[i] = color;
  }
}

void staTick() {
  if (transStep > 0) {
    initStatic(); // for transition
    transFlag = true;
  } else if (transFlag) {
    transFlag = false; // transition end, last set
    initStatic();
  }
}

#endif // STATIC
