#include "Rail.h"
#include "SerialCommand.h"
#include "Adafruit_VL6180X.h"
#include <movingAvg.h>

Rail longRail;
Rail shortRail;
SerialCommand sCmd;
Adafruit_VL6180X vl = Adafruit_VL6180X();
movingAvg distance(10);  
unsigned long nextFocusTime;

enum targetOperation {
  stopped,
  goHomeOperation,
  findFocusOperation,
  moveRailOperation
};

int coreSurface;

targetOperation currentOperation;

void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  
  Serial.println("Rebooting");
  sCmd.addCommand("H", goHome);
  sCmd.addCommand("M", moveRail);
  sCmd.addCommand("R", runRail);
  sCmd.addCommand("S", stopRail);
  sCmd.addCommand("F", findFocus);
  sCmd.addCommand("P", takePhoto);
  sCmd.addCommand("I", getInterrupt);
  shortRail.init('S', 32, 31, 19, 3);
  longRail.init('L', 34, 35, 18, 19);
  attachInterrupt(digitalPinToInterrupt(shortRail._homePin), localISR, CHANGE);
  Serial.println("Ready to go!");
  distance.begin();
  nextFocusTime = micros();
}

void loop() {
  longRail.tick();
  shortRail.tick();
  sCmd.readSerial();
  
  if(nextFocusTime < micros()) {
    updateFocus();
    nextFocusTime = micros() + 10000;
  }

  switch(currentOperation) {
    case goHomeOperation:
      if(longRail.isHome() && shortRail.isHome()) {
        Serial.println("HOME");
        currentOperation = stopped;
      }
      break;
    case findFocusOperation: 
      findFocus();
      if(coreSurface > 0) {
        currentOperation = stopped;
        Serial.print("FOCUS:");
        Serial.println(coreSurface);
      }
      break;
    case moveRailOperation:
      if(shortRail.atPosition() && longRail.atPosition()) {
        Serial.println("POSITIONED");
        currentOperation = stopped;
      }
  }
}

void localISR() {
//  longRail.homeISR();
//  longRail.limitISR();
  shortRail.homeISR();
//  shortRail.limitISR();
  
  
  
}

void findFocus() {
  currentOperation = findFocusOperation;
  coreSurface = 0;
  if(distance.getAvg() < 30.0) {
    shortRail.move(10, 1, 500);
  }
  else if(distance.getAvg() > 32.0) {
    shortRail.move(10, 0, 500);
  }
  else {
    coreSurface = shortRail._currentRotations;
    
  }
  
  
}

void updateFocus() { 
  float lux = vl.readLux(VL6180X_ALS_GAIN_5);
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();
  distance.reading(range);
  if (status == VL6180X_ERROR_NONE) {
//    Serial.print("Range: "); Serial.println(distance.getAvg());
  }

  

//  // Some error occurred, print it out!
//  
//  if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
//    Serial.println("System error");
//  }
//  else if (status == VL6180X_ERROR_ECEFAIL) {
//    Serial.println("ECE failure");
//  }
//  else if (status == VL6180X_ERROR_NOCONVERGE) {
//    Serial.println("No convergence");
//  }
//  else if (status == VL6180X_ERROR_RANGEIGNORE) {
//    Serial.println("Ignoring range");
//  }
//  else if (status == VL6180X_ERROR_SNR) {
//    Serial.println("Signal/Noise error");
//  }
//  else if (status == VL6180X_ERROR_RAWUFLOW) {
//    Serial.println("Raw reading underflow");
//  }
//  else if (status == VL6180X_ERROR_RAWOFLOW) {
//    Serial.println("Raw reading overflow");
//  }
//  else if (status == VL6180X_ERROR_RANGEUFLOW) {
//    Serial.println("Range reading underflow");
//  }
//  else if (status == VL6180X_ERROR_RANGEOFLOW) {
//    Serial.println("Range reading overflow");
//  }
}

void goHome() {
  shortRail.goHome();
  longRail.goHome();
  currentOperation = goHomeOperation;
}

void getInterrupt() {
  Serial.print("Short Rail (H/L) ");
  Serial.print(shortRail._homeInterrupt);
  Serial.print(" ");
  Serial.println(shortRail._homeInterrupt);
  Serial.print("Long Rail (H/L) ");
  Serial.print(longRail._homeInterrupt);
  Serial.print(" ");
  Serial.println(longRail._homeInterrupt);
  
}

void moveRail() {
  currentOperation = moveRailOperation;
  char *rail;
  rail = sCmd.next();

  char *direction;
  direction = sCmd.next();

  char *distance;
  distance = sCmd.next();
  char *speed;
  speed = sCmd.next();  
  if(*rail == 'S') {
    shortRail.moveRail(atoi(distance), atoi(direction), atoi(speed));
  }
  else {
    longRail.moveRail(atoi(distance), atoi(direction), atoi(speed));
  }
}

void runRail() {

  char *rail;
  rail = sCmd.next();
  char *direction;
  direction = sCmd.next();
  char *speed;
  speed = sCmd.next();  
  if(*rail == 'S') {
    shortRail.moveRail(99999, atoi(direction), atoi(speed));
  }
  else {
    longRail.moveRail(999999, atoi(direction), atoi(speed));
  }
}

void stopRail() {
  char *rail;
  rail = sCmd.next();
  if(*rail == 'S') {
    shortRail.stop();
  }
  else {
    longRail.stop();
  }
  currentOperation = stopped;
}


void takePhoto() {
  
}
