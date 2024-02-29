#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <EEPROM.h>

#define   MESH_PREFIX     "lightMeshC" // name of internal mesh wifi network
#define   MESH_PASSWORD   "G3t0ut0fmyPr0p3rty"
#define   MESH_PORT       5555 // should not be important, default 5555

#define LED_TYPE WS2812 // i'm using WS2812, other possible are APA102, WS2801...
#define COLOR_ORDER GRB // default is GRB for WS2812

#define ZONE_BITMAP 0b00000001 // zone definition, possible values for zoning BITMAP (1-2-4-8-16-32...)
#define SHORT 0 // short version
#define ENABLE_SYNC 1 // use sync
#define ESP32 1 // ESP version (1=MH ET LIVE ESP32MiniKit / 0= WEMOS D1 mini)
#define SERVER 0 // disable lights and tries to be root of mesh
#define HAS_SERVER_IN_MESH 0 //tries to NOT be root if using server mode
#define READ_SERIAL 1 // serial reader - SHOULD BE ACTIVE WHEN SERVER = 1
#define DEFAULT_ON 0 // turn on at startup
#define USE_DOUBLE_CLICK 1 // double click action
#define DOUBLE_CLICK_LEN 20 // 20 default
#define TOUCH_THRESHOLD 45 // 45 default, higher values mean less sensitive
#define TOUCH_TICK 2 // 3 default, minimum number of ticks (15ms) to filter out false positives
#define TR_SPEED 12 // 12 default, transition speed between effects - colors - on/off

#define EFFECT_OFF 0 // static with color black
#define EFFECT_STATIC 1 // static with main color
#define EFFECT_SNAKE 2 // main color background with secondary colored strips moving up and down
#define EFFECT_RAINBOW 3 // infamous RGB puke
#define EFFECT_NB 4 // numbers of effects

#define MEM_SIZE 32 // size of persistent memory in bytes. addresses are per byte, taking into consideration previous adresses types size.
//first int is for initialisation detection purpose. check 'iniCheck' verification
#define ADR_C0 4 // main color
#define ADR_C1 8 // secondary color
#define ADR_NB 12 // number of LEDs - unused
#define ADR_BR 16 // brightness
#define ADR_EF 20 // current effect

#if ESP32
  #define LED_DT 17 // Data pin to connect to the strip. default is 17 for ESP32 (IO17)
  #define BTN_DT 33 // Data pin to connect to the Touch sensor. default is 33 (IO33)
#else // 8266
  #define LED_DT D3 // Data pin to connect to the strip. default is D3 for ESP8266
  #define BTN_DT D5 // Data pin to connect to the Button. default is D5 for ESP8266
#endif

// default colors on first init
CRGB COLOR_MAIN  = CRGB(0x6C00CD); 
CRGB COLOR_SEC = CRGB(0x00DCDC);

uint8_t max_bright = 100; // initial Overall brightness. I'd avoid going over 128 for long term usage because of heat.

const int MAX_NUM_LEDS = 100;
#if SHORT
  int NUM_LEDS = 15;               // Initial Total Number of LED's for short version.
#else // tall
  int NUM_LEDS = 35;               // Initial Total Number of LED's for tall version.
#endif

int currentEffect = DEFAULT_ON ? EFFECT_OFF : EFFECT_SNAKE; // default
int nextEffect = EFFECT_SNAKE;
bool onoff = (DEFAULT_ON == 1);
int transStep = 0;


// SNAKE
#define SNAKE_NB_MAX 5
#define SNAKE_LEN_MIN 6
#define SNAKE_LEN_MAX 64
#define SNAKE_SPEED_MAX 1.0

#if SHORT
  #define SNAKE_LEN_FADE 4
  int SNAKE_NB = 1;              // snake number
  int SNAKE_LEN = 10;            // snake led length
#else // tall
  #define SNAKE_LEN_FADE 5
  int SNAKE_NB = 2;              // snake number
  int SNAKE_LEN = 12;            // snake led length
#endif

float SNAKE_SPEED = 0.07;      // *SNAKE_NB must be <1.0

void initParams(){
  EEPROM.begin(MEM_SIZE);
  int c0, c1, ef;
  int iniCheck;
  EEPROM.get(0, iniCheck);
  if (iniCheck != 42) {
    // never initialized
    EEPROM.put(ADR_C0, 0x82E6EC);
    EEPROM.put(ADR_C1, 0xDA00C0);
    //EEPROM.put(ADR_NB, NUM_LEDS);
    EEPROM.put(ADR_BR, max_bright);
    EEPROM.put(ADR_EF, currentEffect);

    EEPROM.put(0, 42);
    EEPROM.commit();
  } else {
    EEPROM.get(ADR_C0, c0);
    EEPROM.get(ADR_C1, c1);
    //EEPROM.get(ADR_NB, NUM_LEDS);
    EEPROM.get(ADR_BR, max_bright);
    EEPROM.get(ADR_EF, ef);
  }
//  EEPROM.put(ADR_NB, NUM_LEDS);
//  EEPROM.commit();

  COLOR_MAIN = CRGB(c0);
  COLOR_SEC = CRGB(c1);
  currentEffect = ef;
  nextEffect = ef;
  
}

void setParamColor(int index, int color) {
  switch(index) {
    case 0: EEPROM.put(ADR_C0, color); COLOR_MAIN = CRGB(color); break;
    case 1: EEPROM.put(ADR_C1, color); COLOR_SEC = CRGB(color); break;
  }
  EEPROM.commit();
}

void setNextEffect(int ef) {
  EEPROM.put(ADR_EF, ef);
  nextEffect = ef;
  EEPROM.commit();
}

void setCurrentEffect(int ef) {
  currentEffect = ef;
}

void setBrightness(uint8_t br){
  max_bright = br;
  EEPROM.put(ADR_BR, br);
  EEPROM.commit();
  // might need reboot
}


#endif //PARAMETERS_H
