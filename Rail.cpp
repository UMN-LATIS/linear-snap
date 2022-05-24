
#include "Rail.h"
Rail::Rail() {
  
}

void Rail::init(int stepPin, int dirPin, int homePin, int limitPin) {
    _stepPin = stepPin;
    _dirPin = dirPin;
    _homePin = homePin;
    _limitPin = limitPin;
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);

//    _homeInterruptButton = EasyButton(_homePin, 10, false, false);
//    _homeInterruptButton.begin();
//    _limitInterruptButton = EasyButton(this->_limitPin, 10, false, false);
//    _limitInterruptButton.begin();
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

    moveRail(999999, directionHome, fast);
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

bool Rail::moveRail(long rotations, int moveDirection, int speed) {
    digitalWrite(_dirPin, moveDirection);
    _targetRotations = rotations;
    _currentRotations = 0;
    _currentDirection = moveDirection;
    _moveSpeed = speed;
    return true;
}

void Rail::tick() {
  if(_currentRotations < _targetRotations && millis() >= _nextActionTime) {
    if(_limitInterrupt == true && _currentDirection == directionAway) {
          digitalWrite(_stepPin, LOW);  
          _currentStatus = LOW;
          return;
      }
      if(_homeInterrupt == true && _currentDirection == directionHome) {
          digitalWrite(_stepPin, LOW);  
          _currentStatus = LOW;
          return;
      }
      
      _limitInterrupt = false;
      _homeInterrupt = false;

      if(_currentStatus == LOW) {
        digitalWrite(_stepPin, HIGH);
        _currentStatus = HIGH;  
      }
      else {
        digitalWrite(_stepPin, LOW);  
        _currentStatus = LOW;
        _currentRotations++;
        if(_currentDirection == directionHome) {
          _current_position--;
        }
        else {
          _current_position++;
        }
      }
      
      _nextActionTime = millis() + _moveSpeed;  
      
      
      
  }
}
