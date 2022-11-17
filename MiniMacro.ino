#include "Rail.h"
#include "SerialCommand.h"
#include "Adafruit_VL6180X.h"
#include <movingAvg.h>


#define short_home_pin 19
#define short_interrupt_pin 3
#define long_home_pin 2
#define long_interrupt_pin 18

#define short_step_pin 32
#define short_dir_pin 31
#define long_step_pin 34
#define long_dir_pin 35

#define focus_average_count 10

Rail longRail;
Rail shortRail;
SerialCommand sCmd;
Adafruit_VL6180X vl = Adafruit_VL6180X();
unsigned long nextFocusTime;
movingAvg distance(focus_average_count);



enum targetOperation {
  stopped,
  goHomeOperation,
  findFocusOperation,
  moveRailOperation,
  runRailOperation
};

int coreSurface;
int focusSampleCount = 0;

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

  vl.startRangeContinuous(50);
  
  Serial.println("Rebooting");
  sCmd.addCommand("H", goHome);
  sCmd.addCommand("M", moveRail);
  sCmd.addCommand("R", runRail);
  sCmd.addCommand("S", stopRail);
  sCmd.addCommand("F", findFocus);
  sCmd.addCommand("G", getFocus);
  sCmd.addCommand("P", takePhoto);
  sCmd.addCommand("I", getInterrupt);
  shortRail.init('S', short_step_pin, short_dir_pin, short_home_pin, short_interrupt_pin);
  longRail.init('L', long_step_pin, long_dir_pin, long_home_pin, long_interrupt_pin);
  attachInterrupt(digitalPinToInterrupt(shortRail._homePin), localISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(shortRail._limitPin), localISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(longRail._homePin), localISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(longRail._limitPin), localISR, CHANGE);
  Serial.println("Ready to go!");
  distance.begin();
  nextFocusTime = micros();
}

void loop() {
  longRail.tick();
  shortRail.tick();
  sCmd.readSerial();
  long focusReadTime = 50000;

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
      break;
    case runRailOperation:
      focusReadTime = 999999999;
      if(shortRail.atPosition() && longRail.atPosition()) {
        Serial.println("POSITIONED");
        currentOperation = stopped;
      }
      break;
  }

   
  if(nextFocusTime < micros()) {
    updateFocus();
    nextFocusTime = micros() + focusReadTime;
  }
}

void localISR() {
  
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    longRail.homeISR();
    longRail.limitISR();
    shortRail.homeISR();
    shortRail.limitISR();
  }
  last_interrupt_time = interrupt_time;
  
}

void getFocus() {
  updateFocus();
  Serial.println(distance.getAvg());
}

void findFocus() {
  currentOperation = findFocusOperation;
  if(!shortRail.atPosition()) {
    return;
  }

  coreSurface = 0;
  float average = distance.getAvg();
  if(average < 140.0) {
    if(average < 120.0) {
      shortRail.moveRail(100, 0, 2000);
    }
    else {
      shortRail.moveRail(20, 0, 2000);  
    }
    
  }
  else if(average > 145.0) {
    if(average > 165.0) {
      shortRail.moveRail(100, 1, 2000);
    }
    else {
      shortRail.moveRail(20, 1, 2000);  
    }
    
  }
  else {
    coreSurface = shortRail._currentRotations;
    currentOperation = stopped;
    
  }
  
}

void updateFocus() { 
  uint8_t range = vl.readRangeResult();
  distance.reading(range);
}

void goHome() {
  shortRail.goHome();
  longRail.goHome();
  currentOperation = goHomeOperation;
}

void getInterrupt() {
  localISR();
  Serial.print("Short Rail (H/L) ");
  Serial.print(shortRail._homeInterrupt);
  Serial.print(" ");
  Serial.println(shortRail._limitInterrupt);
  Serial.print("Long Rail (H/L) ");
  Serial.print(longRail._homeInterrupt);
  Serial.print(" ");
  Serial.println(longRail._limitInterrupt);
  
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
  currentOperation = runRailOperation;
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
