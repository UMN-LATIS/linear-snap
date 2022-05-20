#include "Rail.h"

Rail longRail;
Rail shortRail;

void setup() {
  // put your setup code here, to run once:
  longRail.init(2, 3, 7, 8);
  shortRail.init(4,5,9, 10);

  longRail._homeInterruptButton.enableInterrupt(localISR);
  longRail._limitInterruptButton.enableInterrupt(localISR);
  shortRail._homeInterruptButton.enableInterrupt(localISR);
  shortRail._limitInterruptButton.enableInterrupt(localISR);
  longRail.goHome();
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
