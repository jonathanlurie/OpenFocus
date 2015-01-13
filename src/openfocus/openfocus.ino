/*
OpenFocus
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


#include <Joystick.h>
#include "SimpleStepper.h"

// the basic delay to wait
const int BASIC_DELAY = 3;

// if under this value, we consider the joystick as static
const float STICK_THRESH = 0.02;


// JOYSTICK
const int _verticalPin = 5;  // analog
const int _horizontalPin = 6;  // analog
const int _selectorPin = 2;  // digital, allow Interrupt
long _chronoThresh; // default is 500ms, but can be set
Joystick *_focusStick;


// STEPPER
// the pin for direction
const int _motorDirPin = 4;
// the pin asking to perform a step
const int _motorStepPin = 5;
// number of step in one entire revolution
// note: this is not mandatory when performing solo steps
const int _totalNbStep360 = 200;
// define the speed of rotation in RPM
// note: this is not mandatory when performing solo steps
const float _speedRP = 60;
SimpleStepper _motor(_motorDirPin, _motorStepPin);
// number of steps to move from reccorded position
int _stepsToMove = 0;


// LED
const int _ledPin = 13;
long _chronoLedStart;


void setup() {

  // SERIAL
  Serial.begin(57600);

  // JOYSTICK
  _focusStick = new Joystick(_selectorPin, _horizontalPin, _verticalPin);
  _chronoThresh = _focusStick->getPressTimeThreshold();

  // joystick pushbutton is mapped on interruption
  attachInterrupt(0, manageChrono, CHANGE);

  // STEPPER
  _motor.setTotalNbSteps(_totalNbStep360);
  _motor.setRPM(_speedRP);

  // LED
  pinMode(_ledPin, OUTPUT);
  digitalWrite(_ledPin, HIGH);
}

void loop() {


  // reading the focus joystick value
  float focusStickNormValue = _focusStick->getNormalizedVertical();


  // TODO
  // reading influence potentiometer value
  float influence = 1.0;

  // TODO
  // reading the switch to revert rotation
  boolean revertRotation = false;


  // the stick is pushed or pulled
  if(abs(focusStickNormValue ) > STICK_THRESH){

    // XOR between stick rotation direction and the switch that reverts
    // TODO : doesnt work when revert is true!!
    if(!(focusStickNormValue > 0) != !revertRotation){
      _motor.setDirectionClockWise();

      // in case of reccording
      _stepsToMove ++;
    }else{
      _motor.setDirectionCounterClockWise();

      // in case of reccording
      _stepsToMove --;
    }

          Serial.println(_stepsToMove);

    // final delay between twoo movings
    long movingDelay = (float)BASIC_DELAY * 1000.0 * influence * (1.0 / abs(focusStickNormValue) );

    //Serial.println(movingDelay);

    _motor.renegateQuarterStep();
   // _motor.renegateStep();

    if(movingDelay > 2000){
      delay((int)(movingDelay/1000.));
    }else{
      delayMicroseconds(movingDelay);
    }


  }else{

    // the led might be switched off because of a long press on the joystick
    // if longer than, it must be switched on
    if((millis() - _chronoLedStart) > _chronoThresh && _chronoLedStart != 0){
      digitalWrite(_ledPin, HIGH);
      delay(100);
      digitalWrite(_ledPin, LOW);
      delay(100);
      digitalWrite(_ledPin, HIGH);
      _chronoLedStart = 0;
    }

    // the stick is idle, just wait...
    delay(50);
  }

}


// deals with the joystick push button and go back to saved position
void manageChrono(){

  if(_focusStick->isPressed()){  // PRESS event
    // blink the led
    digitalWrite(_ledPin, LOW);
    _chronoLedStart = millis();

    Serial.println("start chrono");
    _focusStick->startReccordTime();

  }else{  // RELEASE event
    // unblink the led
    digitalWrite(_ledPin, HIGH);

    Serial.println("stop chrono");
    boolean isLongPressed = _focusStick->stopReccordTime();
    Serial.println(isLongPressed);


    if(isLongPressed){ // LONG PRESS
      // reinitialize the position to reccord this one
      _stepsToMove  = 0;

    }else{  // SHORT PRESS
      // we cancel the confirmation led blink
      _chronoLedStart = 0;

      // does not move if does not need
      if(_stepsToMove == 0){
        return;
      }

      Serial.println(_stepsToMove);

      // go to the position previously reccorded

      // choose the direction (not the shortest, but the real one!)
      if(_stepsToMove < 0){
        _motor.setDirectionClockWise();
      }else{
        _motor.setDirectionCounterClockWise();
      }

      // performs the rotations
      _motor.rotationSteps(abs(_stepsToMove/4));

      // reset the counter
      _stepsToMove = 0;

    }

  }

}
