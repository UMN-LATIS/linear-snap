#include "Rail.h"
#include "SerialCommand.h"


#define short_home_pin 2
#define short_interrupt_pin 3
#define long_home_pin 18
#define long_interrupt_pin 17
//le
 #define short_step_pin 5
#define short_dir_pin 4
 #define long_step_pin 7
 #define long_dir_pin 6
#define photo_pin 8

#define led_pin 19
//#define short_step_pin 32
//#define short_dir_pin 31
//#define long_step_pin 34
//#define long_dir_pin 35
//#define photo_pin 45

#define DEBOUNCE_TIME 10


Rail longRail;
Rail shortRail;
SerialCommand sCmd;

long previousTime;

enum targetOperation {
  stopped,
  goHomeOperation,
  moveRailOperation,
  runRailOperation
};


targetOperation currentOperation;

void setup() {
  
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
//  if (! vl.begin()) {
//    Serial.println("Failed to find sensor");
//    while (1);
//  }

//  vl.startRangeContinuous(50);
  
  Serial.println("Rebooting");
  sCmd.addCommand("H", goHome);
  sCmd.addCommand("M", moveRail);
  sCmd.addCommand("R", runRail);
  sCmd.addCommand("S", stopRail);
  sCmd.addCommand("P", takePhoto);
  sCmd.addCommand("I", getInterrupt);
  sCmd.addCommand("L0", lightOff);
  sCmd.addCommand("L1", lightOn);
  shortRail.init('S', short_step_pin, short_dir_pin, short_home_pin, short_interrupt_pin);
  longRail.init('L', long_step_pin, long_dir_pin, long_home_pin, long_interrupt_pin);
  pinMode(short_home_pin, INPUT_PULLUP);
  pinMode(short_interrupt_pin, INPUT_PULLUP);
  pinMode(long_interrupt_pin, INPUT_PULLUP);
  pinMode(long_home_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);
  
  attachInterrupt(digitalPinToInterrupt(short_home_pin), shortHomeISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(short_interrupt_pin), shortLimitISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(long_home_pin), longHomeISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(long_interrupt_pin), longLimitISR, CHANGE);
  delay(1000);
  Serial.println("reading interrupts");
  getInterrupt();

  Serial.println("Ready to go!");
  pinMode(photo_pin, OUTPUT);
  digitalWrite(photo_pin, LOW);
}

void loop() {
  longRail.tick();
  shortRail.tick();
  sCmd.readSerial();
  

  switch(currentOperation) {
    case goHomeOperation:
      if(longRail.isHome() && shortRail.isHome()) {
        Serial.println("HOME");
        currentOperation = stopped;
      }
      break;
    case moveRailOperation:
      if(shortRail.atPosition() && longRail.atPosition()) {
        Serial.println("POSITIONED");
        currentOperation = stopped;
      }
      break;
    case runRailOperation:
      if(shortRail.atPosition() && longRail.atPosition()) {
        Serial.println("POSITIONED");
        currentOperation = stopped;
      }
      break;
  }

}

bool okToInterrupt() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
     return true;
  }
  return false;
  
}

// Define the ISRs
void shortHomeISR() {
    if(okToInterrupt()) {
      shortRail.homeISR();
    }
}

void shortLimitISR() {
    if(okToInterrupt()) {
      shortRail.limitISR();
    }
}

void longHomeISR() {
    if(okToInterrupt()) {
      longRail.homeISR();
    }
  
}

void longLimitISR() {
    if(okToInterrupt()) {
      longRail.limitISR();
    }
 
}



void goHome() {
  shortRail.goHome(shortRail.slow);
  longRail.goHome(longRail.fast);
  currentOperation = goHomeOperation;
}

void getInterrupt() {
  shortHomeISR();
  shortLimitISR();
  longHomeISR();
  longLimitISR();
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

void lightOn() {
  digitalWrite(led_pin, LOW);
}

void lightOff() {
  digitalWrite(led_pin, HIGH);
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
  Serial.println("start capture");
  digitalWrite(photo_pin, HIGH);
  delay(200); // May want to adjust this depending on shot type
  digitalWrite(photo_pin, LOW);
//  Serial.println("end capture");
}
