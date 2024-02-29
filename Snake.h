#ifndef SNAKE_H
#define SNAKE_H

#include "Common.h"

const uint16_t snakeInitialPos[5] = {0, 10, 20, 30, 40};
#if SHORT
  const uint8_t snakeMap[SNAKE_LEN_FADE] = {0,85,171,255};
#else // tall
  const uint8_t snakeMap[SNAKE_LEN_FADE] = {0,64,128,192,255};
#endif

typedef struct {
  uint16_t snakePos[SNAKE_LEN_MAX]; // [0] is head
  float shift = 0.0;
  float ledPerTick = 0.3;
  bool dir = true;
} SNAKE;

SNAKE snakes[SNAKE_NB_MAX];

void initSnake() {
  Serial.println("Snake init");
  for (int j = 0; j<SNAKE_NB; ++j) {
    snakes[j].ledPerTick = SNAKE_SPEED + (0.05*j);
    snakes[j].dir = (j % 2 == 0);
    for (int i = 0; i<SNAKE_LEN; ++i) {
      snakes[j].snakePos[i] = (snakeInitialPos[j%5] + i) % NUM_LEDS;
    }
  }
  
} //initSnake

uint8_t calculerp(int pos, float decal) {
  if (pos < SNAKE_LEN_FADE - 1) {
    return snakeMap[pos] + decal * (snakeMap[pos+1] - snakeMap[pos]);
  } else if (pos > SNAKE_LEN - SNAKE_LEN_FADE) {
    return snakeMap[SNAKE_LEN - pos] + decal * (snakeMap[SNAKE_LEN - (pos+1)] - snakeMap[SNAKE_LEN - pos]);
  } else {
    return 255;
  }
} // calculerp


void setNextPos(SNAKE &sn) {
  sn.shift += sn.ledPerTick;
    if ( sn.shift >= 1.0) { // need to move
      uint16_t prevHead = sn.snakePos[0];
      for (int i = SNAKE_LEN -1; i>0; --i) { // move all toward head
        sn.snakePos[i] = sn.snakePos[i-1];
      }
      if (sn.dir) {
        sn.snakePos[0] = (sn.snakePos[0] + 1) % NUM_LEDS ;
      } else {
        sn.snakePos[0] = (sn.snakePos[0] == 0 ? NUM_LEDS - 1 : sn.snakePos[0] - 1);
      }

      sn.shift -= 1.0;
    }
}

void snakeTick() {
  //Serial.println("Snake tick ");
  // set background
  for (int i = 0; i<NUM_LEDS; ++i) {
    leds[i] = COLOR_MAIN;
  }

  for (int s = 0; s < SNAKE_NB; ++s) {
    SNAKE &sn = snakes[s];

    sn.ledPerTick = SNAKE_SPEED + (0.05*s);
    setNextPos(sn);

    //apply color

    for (int i = 0; i<SNAKE_LEN; ++i) {
      leds[sn.snakePos[i]] = blend(leds[sn.snakePos[i]], calculerp(i,sn.shift), COLOR_SEC);
    }
  }
} // snake()

#endif //SNAKE_H
