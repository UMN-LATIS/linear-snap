
#include "Rail.h"
Rail::Rail() {
  
}

void Rail::init(char rail, int stepPin, int dirPin, int homePin, int limitPin) {
    _rail = rail;
    _stepPin = stepPin;
    _dirPin = dirPin;
    _homePin = homePin;
    _limitPin = limitPin;
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);

    _homeInterruptButton = Button(_homePin);
    _homeInterruptButton.begin();
    _homeInterrupt = _homeInterruptButton.read();
    _limitInterruptButton = Button(_limitPin);
    _limitInterruptButton.begin();
//    _limitInterrupt = _limitInterruptButton.read();

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
        Serial.println("Home:" + _rail);
        return true;
    }
    return false;
}

bool Rail::atPosition() {
  if(_currentRotations == _targetRotations) {
    return true;
  }
  return false;
}

void Rail::homeISR() {
    int oldState = _homeInterrupt;
    _homeInterrupt = _homeInterruptButton.read();
    if(_homeInterrupt != oldState) {
      _homePosition = _current_position;
      _targetRotations = 0;
    }
}

void Rail::limitISR()
{
    int oldState = _limitInterrupt;
    _limitInterrupt = _limitInterruptButton.read();
    if(_limitInterrupt != oldState) {
    }
}

bool Rail::moveRail(long rotations, int moveDirection, int speed) {
    digitalWrite(_dirPin, moveDirection);
    _targetRotations = rotations;
    _currentRotations = 0;
    _currentDirection = moveDirection;
    _moveSpeed = speed;
    _nextActionTime = 0;
    return true;
}

void Rail::tick() {
  if(_currentRotations < _targetRotations && micros() >= _nextActionTime) {
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

      _nextActionTime = micros() + _moveSpeed;  
      
      
  }

  if(_currentRotations >= _targetRotations) {
  }
}

void Rail::stop() {
  _currentRotations = _targetRotations;
  _nextActionTime = 0;
}
