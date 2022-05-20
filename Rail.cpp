
#include "Rail.h"
Rail::Rail() {
  
}

void Rail::init(int stepPin, int dirPin, int homePin, int limitPin) {
    _stepPin = stepPin;
    _dirPin = dirPin;
    _homePin = homePin;
    _limitPin = limitPin;

    _homeInterruptButton = EasyButton(_homePin, 10, false, false);
    _homeInterruptButton.begin();
    _limitInterruptButton = EasyButton(this->_limitPin, 10, false, false);
    _limitInterruptButton.begin();
//    _limitInterruptButton.enableInterrupt(limitISR);
    // pinMode(_stepPin, OUTPUT);
    // pinMode(_dirPin, OUTPUT);
    // pinMode(_homePin, INPUT);
    // pinMode(_limitPin, INPUT);
    // digitalWrite(_stepPin, LOW);
    // digitalWrite(_dirPin, LOW);
    _current_position = 0;
}

void Rail::goHome() {
    if(isHome()) {
        return;
    }

    moveRail(999999, directionHome);
}

bool Rail::isHome() {
    if(_current_position == _homePosition) {
        return true;
    }
    return false;
}

void Rail::homeISR() {
    _homeInterrupt = _homeInterruptButton.read();
    if(_homeInterrupt) {
      _homePosition = _current_position;
    }
}

void Rail::limitISR()
{
    _limitInterrupt = _limitInterruptButton.read();
}

bool Rail::moveRail(long rotations, int moveDirection) {
    digitalWrite(_dirPin, moveDirection);
    _targetRotations = rotations;
    _currentRotations = 0;
    _currentDirection = moveDirection;
    return true;
}

void Rail::tick() {
  if(_currentRotations < _targetRotations) {
    if(_limitInterrupt == true && _currentDirection == directionAway) {
          return;
      }
      if(_homeInterrupt == true && _currentDirection == directionHome) {
          return;
      }
      _limitInterrupt = false;
      _homeInterrupt = false;
      digitalWrite(_stepPin, HIGH);
      delayMicroseconds(1000);
      digitalWrite(_stepPin, LOW);
      delayMicroseconds(1000);
      _currentRotations++;
      if(_currentDirection == directionHome) {
        _current_position--;
      }
      else {
        _current_position++;
      }
  }
}
