#include "painlessMesh.h"
#include "FastLED.h"                                          // FastLED library.
#include "Common.h"
#include "Static.h"
#include "Snake.h"
#include "Rainbow.h"

#if ESP32
  int currentTouchTick = 0;
  TaskHandle_t Core0Task;
#endif


Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
size_t serverId = 0;
int buttonState = 0;
int clickTimer = 0;
int clickFlag = 0;
bool startTouch = false;


#if READ_SERIAL
 String readSerial = "";
#endif

// Prototypes
bool receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
#if ESP32
  void meshTask(void *parameter);
#endif

void singleClick(bool sendToOthers){
  //Serial.printf("CLICK\n");
  onoff = !onoff;
  initTransition();
  if (!onoff) {
    //Serial.printf("OFF\n");
    initStatic();
  } else {
    //Serial.printf("ON\n");
    switch(currentEffect) {
    case EFFECT_SNAKE:
      initSnake();
      snakeTick();
      break;
    case EFFECT_OFF:
    case EFFECT_STATIC:
      initStatic();
      staTick();
      break;
    }
  }
  #if ENABLE_SYNC
    if (sendToOthers) {
      DynamicJsonDocument jsonBuffer(1024);
      JsonObject msg = jsonBuffer.to<JsonObject>();
    
      msg["topic"] = "onoff";
      msg["onoff"] = onoff;
    
      String str;
      serializeJson(msg, str);
      mesh.sendBroadcast(str);
    }
  #endif
}

void doubleClick(bool sendToOthers){
  initTransition();
  setNextEffect(1 + (currentEffect % (EFFECT_NB - 1)));
  #if ENABLE_SYNC
    if (sendToOthers) {
      DynamicJsonDocument jsonBuffer(1024);
      JsonObject msg = jsonBuffer.to<JsonObject>();
    
      msg["topic"] = "mode";
      msg["effect"] = nextEffect;
    
      String str;
      serializeJson(msg, str);
      mesh.sendBroadcast(str);
    }
  #endif
}

void checkClick(){
  #if ESP32
    //buttonState = touchRead(BTN_DT) > TOUCH_THRESHOLD ? 1 : 0;
    //Serial.printf("touch: %d \n", touchRead(BTN_DT));
    if (!startTouch && millis() > 1000) {
      startTouch = true;
    }
    if (startTouch && touchRead(BTN_DT) < TOUCH_THRESHOLD) {
      ++currentTouchTick;
    } else {
      if (currentTouchTick != 0) {
        Serial.printf("touch length: %d \n", currentTouchTick);
      }
      currentTouchTick = 0;
    }
    buttonState = currentTouchTick > TOUCH_TICK ? 0 : 1;

  #else // 8266
    buttonState = digitalRead(BTN_DT);
  #endif // ESP
  if (buttonState == LOW && clickFlag == 0) {
    if (clickTimer > 3){
      #if USE_DOUBLE_CLICK
        doubleClick(true);
      #else
        singleClick(true);
      #endif
      clickTimer = 0;
    } else {
      clickTimer++;
    }
    clickFlag = 1;
  }
  if (buttonState == HIGH && clickFlag == 1) { 
    clickFlag = 0;
  }
  if (clickTimer > 0){
    if (clickTimer > DOUBLE_CLICK_LEN) {
      clickTimer = 0;
      singleClick(true);
    } else {
      clickTimer++;
    }
  }
}


void setup() {
  #if ESP32
    setCpuFrequencyMhz(80);
  #endif
  Serial.begin(115200);
  #if ENABLE_SYNC
    mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
    mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6, 0);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
  #endif
  initParams();
  #if !(SERVER)
    pinMode(BTN_DT, INPUT_PULLUP);  
    LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(0xFFB0F0);  // Use this for WS2812
    FastLED.setBrightness(max_bright);

    if (onoff) {
      //Serial.printf("START ON \n");
      switch(currentEffect) {
        case EFFECT_SNAKE:
          initSnake();
          snakeTick();
          break;
        case EFFECT_OFF:
        case EFFECT_STATIC:
          initStatic();
          staTick();
          break;
      }
      if (currentEffect != nextEffect) {
        initTransition();
      }
    } else {
      //Serial.printf("START OFF \n");
      initStatic();
    }
    FastLED.show();
    #if ESP32
      xTaskCreatePinnedToCore(
        core0Task,
        "Core 0 task",
        10000,
        NULL,
        1,
        &Core0Task,
        0
      );
    #endif // ESP32
  #else // SERVER
    mesh.setRoot(true);
  #endif // !SERVER
  #if HAS_SERVER_IN_MESH
    mesh.setContainsRoot(true);
  #endif
  Serial.printf("INIT END \n");
}

#if READ_SERIAL
void readInput() {
  while (Serial.available() > 0) {
    char rc = Serial.read();
    if (rc != '\n') {
      readSerial += rc;
    } else {
      if (receivedCallback(0, readSerial)) {
        #if ENABLE_SYNC
          mesh.sendBroadcast(readSerial);
          #if !(ESP32)
            mesh.update();
          #endif
        #endif
        DynamicJsonDocument jsonBuffer(1024);
        JsonObject msg = jsonBuffer.to<JsonObject>();
      
        msg["onoff"] = onoff;
        String str;
        serializeJson(msg, str);
        Serial.printf("%s\n", str.c_str());
      }
      readSerial = "";
    }
  }
}
#endif

#if ESP32
void core0Task(void *parameter) {
  for (;;) {
#else
void loop() {
  #if READ_SERIAL
    readInput();
  #endif
#endif
  #if ESP32
    EVERY_N_MILLISECONDS ( 15 ) { 
      checkClick();
    }
  #endif

  EVERY_N_MILLISECONDS ( 30 ) { 
  #if !(ESP32)
    checkClick();
  #endif
  #if !(SERVER)
    if (onoff) { // ON
      if (currentEffect == nextEffect) {
        switch(currentEffect) {
        case EFFECT_SNAKE:
          snakeTick(); break;
        case EFFECT_RAINBOW:
          rainbowTick(); break;
        case EFFECT_STATIC:
        case EFFECT_OFF:
          staTick(); break;
        }
      } else {
        setCurrentEffect(nextEffect);
        switch(currentEffect) {
        case EFFECT_SNAKE:
          initSnake(); break;
        case EFFECT_RAINBOW:
          initRainbow(); break;
        case EFFECT_STATIC:
          initStatic(); break;
        case EFFECT_OFF:
          initStatic(); break;
        }
      }
    } else { // OFF
      staTick();
    }
    applyTransition();
    FastLED.show();
  #endif
  }
  #if ESP32
    }
  #else
    #if ENABLE_SYNC
      mesh.update();
    #endif
  #endif
}

#if ESP32
void loop() {
  #if READ_SERIAL
    readInput();
  #endif
  #if ENABLE_SYNC
    mesh.update();
  #endif
}
#endif

bool receivedCallback(uint32_t from, String &msg) {
  //Serial.printf("logClient: Received from %u msg=%s\n", from, msg.c_str());

  // Saving logServer
  DynamicJsonDocument jsonBuffer(1024 + msg.length());
  DeserializationError error = deserializeJson(jsonBuffer, msg);
  if (error) {
    Serial.printf("DeserializationError\n");
    return false;
  }
  JsonObject root = jsonBuffer.as<JsonObject>();

  #if !(SERVER)
    if (root.containsKey("zone")) {
      int z = root["zone"];
      if ((z & ZONE_BITMAP) == 0) {
        return false;
      }
    }
  #endif

  if (root.containsKey("effect")) {
    initTransition();
    setNextEffect(root["effect"]);
    //Serial.printf("mode changed \n");
  }
  if (root.containsKey("color")) {
    initTransition();
    for (int i = 0; i < 2; ++i) {
      if (root["color"].containsKey(String(i))) {
        //Serial.printf("color %d\n", i);
        setParamColor(i,root["color"][String(i)]);
      }
    }
    //Serial.printf("color changed \n");
  }
  if (root.containsKey("onoff")) {
    if (!root["onoff"] == onoff) {
      singleClick(false);
    }
    //Serial.printf("onoff changed \n");
  }
  if (root.containsKey("brightness")) {
      setBrightness(root["brightness"]);
      FastLED.setBrightness(root["brightness"]);
    //Serial.printf("onoff changed \n");
  }

  //Serial.printf("Handled from %u msg=%s\n", from, msg.c_str());
  return true;
}

void newConnectionCallback(uint32_t nodeId) {
  // Only as root, send current state to new node
  Serial.printf("New connexion\n");
  if (mesh.isRoot()) {
    Serial.printf("Rooooot\n");
    DynamicJsonDocument jsonBuffer(1024);
    JsonObject msg = jsonBuffer.to<JsonObject>();
  
    msg["onoff"] = onoff;
    msg["effect"] = currentEffect;
    String str;
    serializeJson(msg, str);
    mesh.sendSingle(nodeId, str.c_str());
  }
}
