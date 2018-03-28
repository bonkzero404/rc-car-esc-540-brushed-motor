#include "SPI.h"
#include <RF24.h> 
#include <ArduinoJson.h>   

RF24 radio(7,8);

String dataSerial;
byte address[][6] = {"0"};

typedef struct {
  int cmd;
  int val;
  int mod;
} GameData;

GameData jsonData;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10);
  
  radio.begin();
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_MIN);
  // radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address[0]);
}

void loop() {
  StaticJsonBuffer<200> jsonBuffer;
  
  if (Serial.available() > 0) {
    dataSerial = Serial.readString();
   
    JsonObject& root = jsonBuffer.parseObject(dataSerial);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }

    int cmd = root["cmd"];
    int val = root["val"];
    int mod = root["mod"];

    jsonData.cmd = cmd;
    jsonData.val = val;
    jsonData.mod = mod;

    // Send to nrf24
    radio.write(&jsonData, sizeof(jsonData));
  }
}

