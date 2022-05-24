#include "Rail.h"

Rail longRail;
Rail shortRail;

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  shortRail.init(2, 3, 7, 8);
  longRail.init(4,5,9, 10);
  Serial.print("test");
 
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
}

void localISR() {
  longRail.homeISR();
  longRail.limitISR();
  shortRail.homeISR();
  shortRail.limitISR();
  
}
