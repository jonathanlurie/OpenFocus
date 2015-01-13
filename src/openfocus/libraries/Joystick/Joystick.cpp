/*
Joystick
=============
Copyright (c) 2013, Jonathan LURIE, All rights reserved.
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library.
*/


#include "Joystick.h"


// constructor, associates the pins
Joystick::Joystick(int selectorPin, int horizontalPin, int verticalPin){

  _pressTimeThreshold = 500;

  _verticalPin = verticalPin;
  _horizontalPin = horizontalPin;
  _selectorPin = selectorPin;

  pinMode(_selectorPin, INPUT_PULLUP);


  calibrates();

}


// reads the raw horizontal value (0 to 1023)
int Joystick::readsRawHorizontal(){
  return analogRead(_horizontalPin);
}



// reads the raw vertical value (0 to 1023)
int Joystick::readsRawVertical(){
  return analogRead(_verticalPin);
}



// calibrates the joystick to correct the idle values
void Joystick::calibrates(){
  _idleHorizontal = readsRawHorizontal();
  _idleVertical = readsRawVertical();

  // computes tolerance
  _tolerance = max(abs(_idleHorizontal - 511), abs(_idleVertical - 511));
}



// return horizontal normalized value in [-1, 1]
// and 0 is considered idle
float Joystick::getNormalizedHorizontal(){
  float finalValue = 0;

  float rawValue = (float)readsRawHorizontal();

  // is it too close to the idle position?
  if( abs(rawValue - (float)_idleHorizontal) > _tolerance){

    // pushing the stick
    if(rawValue > _idleHorizontal){

      finalValue = (rawValue - _idleHorizontal) / (1023 - _idleHorizontal);

    }
    // pulling the stick
    else{
      finalValue = (1 - (rawValue / _idleHorizontal)) * - 1;
    }

  }

  return finalValue;

}



// return horizontal normalized value in [-1, 1]
// and 0 is considered idle
float Joystick::getNormalizedVertical(){
  float finalValue = 0;

  float rawValue = (float)readsRawVertical();

  // is it too close to the idle position?
  if( abs(rawValue - (float)_idleVertical) > _tolerance){

    // pushing the stick
    if(rawValue > _idleVertical){

      finalValue = (rawValue - _idleVertical) / (1023 - _idleVertical);

    }
    // pulling the stick
    else{
      finalValue = (1 - (rawValue / _idleVertical)) * - 1;
    }

  }

  return finalValue;

}



boolean Joystick::isPressed(){

  int selectorVal = digitalRead(_selectorPin);

  // pressed is LOW
  if(selectorVal == HIGH){
    return false;
  }else{
    return true;
  }

}




// start the time reccord for selector pressing
void Joystick::startReccordTime(){

  _chronoStart = millis();


}

// stop the time reccord for selector pressing
// returns true if over the threshold
bool Joystick::stopReccordTime(){
  _chronoStop = millis();

  if((_chronoStop - _chronoStart) > _pressTimeThreshold){
    return true;
  }else{
    return false;
  }

}


// just a getter
long Joystick::getPressTimeThreshold(){
  return _pressTimeThreshold;

}

// just a setter
void Joystick::setPressTimeThreshold(long t){

  _pressTimeThreshold = t;
}
