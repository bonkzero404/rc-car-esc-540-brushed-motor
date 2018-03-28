#include "SPI.h"
#include <RF24.h>
#include <Servo.h>

///////////////////////////
// Konfig Setir Power
///////////////////////////
#define SERVO_STRAIGHT 90

////////////////////////////
// Seting Pulse Rate
////////////////////////////
#define THROTTLE_MIN        1500
#define THROTTLE_MAX        2000
#define THROTTLE_BAKE       1000

////////////////////////////
// pin Servo
////////////////////////////
#define servoPin 6

////////////////////////////
// pin Motor
////////////////////////////
#define motorPin 9

RF24 radio(7, 8);

Servo servoSteering, motorDc;

byte address[][6] = {"0"};
int servoPos = SERVO_STRAIGHT;
int throttle = THROTTLE_MIN;

typedef struct {
  int cmd;
  int val;
  int mod;
} GameData;

GameData jsonData;

void setup() {
  Serial.begin(115200);

  // Seting nrf24l01 wireless
  radio.begin();
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_MIN);
  // radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, address[0]);
  radio.startListening();

  // Servo pin attach
  servoSteering.attach(servoPin);
  motorDc.attach(motorPin);
  motorDc.writeMicroseconds(THROTTLE_MIN);
}

void loop() {

  ////////////////////////////////
  // Cek jika ada data diterima
  ////////////////////////////////
  if (radio.available()) {
    while(radio.available()) {
      radio.read(&jsonData, sizeof(jsonData));
    }

    // Kontrol setir
    moveCommand(jsonData.cmd, jsonData.val, jsonData.mod);
  }
}

void moveCommand(int cmd, int val, int mod) {
  
  if (jsonData.cmd == 1) {  
    servoPos = map(val, 0, 180, THROTTLE_MAX, THROTTLE_BAKE);
    servoSteering.write(servoPos);
  }
  
  if (jsonData.cmd == 3) {
    throttle = map(val, 0, 255, THROTTLE_MIN, THROTTLE_MAX);
    motorDc.write(throttle);
  } 
  else if (jsonData.cmd == 4) {
    throttle = map(val, 0, 255, THROTTLE_MIN, THROTTLE_BAKE);
    motorDc.write(throttle);
  } 
}

