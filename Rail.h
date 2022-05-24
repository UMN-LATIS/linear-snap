#ifndef RAIL_H
#define RAIL_H
#include <Arduino.h>
#include <EasyButton.h>


class Rail
{
  private:
      long _current_position;
      int _stepPin;
      int _dirPin;
      int _homePin;
      int _limitPin;
      int _homePosition = 0;
      bool _homeInterrupt = false;
      bool _limitInterrupt = false;
      long _targetRotations = 0;
      long _currentRotations = 0;
      int _currentDirection = 0;
      int _moveSpeed = 1000;
      int _nextActionTime = 0;
      int _currentStatus = LOW;
  
  public:
      EasyButton _homeInterruptButton = 0;
      EasyButton _limitInterruptButton = 0;
      Rail();
      int directionHome = LOW;
      int directionAway = HIGH;
      int fast = 600;
      int slow = 1000;
      int veryslow = 2000;
      int stepsPerRevolution = 200;
      void init(int stepPin, int dirPin, int homePin, int limitPin);
      void goHome();
      bool isHome();
      void homeISR();
      void limitISR();
      bool moveRail(long rotations, int moveDirection, int speed);
      void tick();
};

#endif
