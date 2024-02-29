#ifndef RAINBOW_H
#define RAINBOW_H

#include "Common.h"

uint8_t thisSpeed = 10;
uint8_t deltaHue= 10;

void initRainbow() {
  //Serial.println("Rainbow init");
  uint8_t thisHue = beat8(thisSpeed,255); 
  fill_rainbow(leds, NUM_LEDS, thisHue, deltaHue);  
  FastLED.show();
}

void rainbowTick() {
  uint8_t thisHue = beat8(thisSpeed,255); 
  fill_rainbow(leds, NUM_LEDS, thisHue, deltaHue);            
  FastLED.show();
}

#endif // RAINBOW_H
