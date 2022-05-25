#include "Rail.h"
#include "SerialCommand.h"

Rail longRail;
Rail shortRail;
SerialCommand sCmd;


void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  shortRail.init('S', 2, 3, 7, 8);
  longRail.init('L', 4,5,9, 10);
  Serial.print("HELLO");


  sCmd.addCommand("H", goHome);
  sCmd.addCommand("M", moveRail);
  sCmd.addCommand("R", runRail);
  sCmd.addCommand("S", stopRail);
  sCmd.addCommand("F", findFocus);
  sCmd.addCommand("P", takePhoto);
  
  longRail.moveRail(300, longRail.directionAway, longRail.fast);
    shortRail.moveRail(10, shortRail.directionHome, shortRail.fast);
//  longRail._homeInterruptButton.enableInterrupt(localISR);
//  longRail._limitInterruptButton.enableInterrupt(localISR);
//  shortRail._homeInterruptButton.enableInterrupt(localISR);
//  shortRail._limitInterruptButton.enableInterrupt(localISR);s
//  longRail.goHome();
}

void loop() {
  longRail.tick();
  shortRail.tick();
  sCmd.readSerial();
}

void localISR() {
  longRail.homeISR();
  longRail.limitISR();
  shortRail.homeISR();
  shortRail.limitISR();
  
}

void goHome() {
  Serial.print("HOME");
  shortRail.goHome();
  longRail.goHome();
}

void moveRail() {

  char *rail;
  rail = sCmd.next();
  char *direction;
  direction = sCmd.next();
  char *distance;
  distance = sCmd.next();
  char *speed;
  speed = sCmd.next();  
  if(rail == "S") {
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
  if(rail == "S") {
    shortRail.moveRail(99999, atoi(direction), atoi(speed));
  }
  else {
    longRail.moveRail(999999, atoi(direction), atoi(speed));
  }
}

void stopRail() {
  char *rail;
  rail = sCmd.next();
  if(rail == "S") {
    shortRail.stop();
  }
  else {
    longRail.stop();
  }
}

void findFocus() {
  
}

void takePhoto() {
  
}
