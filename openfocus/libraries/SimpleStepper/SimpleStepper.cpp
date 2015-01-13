/*
SimpleStepper
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

#include "SimpleStepper.h"

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include <avr/io.h>
 #include "WProgram.h"
#endif

// constructor
SimpleStepper::SimpleStepper(int dirPin, int stepPin){
  m_dirPin = dirPin;
  m_stepPin = stepPin;
  m_nbSteps = 200;
  m_direction = LOW;
  m_LIMIT_OF_DELAY_USECONDS = 16383;
  m_isTurning = false;

  pinMode(m_dirPin, OUTPUT);
  pinMode(m_stepPin, OUTPUT);
  digitalWrite(m_dirPin, HIGH);
  digitalWrite(m_stepPin, LOW);
}

// destructor
SimpleStepper::~SimpleStepper(){
}

// set the total number of step for a rotation (default: 200);
// since it works with microstepping, this value will be multiplied by _MICROSTEP_
void SimpleStepper::setTotalNbSteps(int nbSteps){
  m_nbSteps = (long)nbSteps * (long)_MICROSTEP_;
}


// set direction to clock wise
void SimpleStepper::setDirectionClockWise(){
  m_direction = HIGH;
  updateRotationDirection();
}

// set direction to counter clock wise
void SimpleStepper::setDirectionCounterClockWise(){
  m_direction = LOW;
  updateRotationDirection();
}

// private: update direction from m_direction
void SimpleStepper::updateRotationDirection(){
  digitalWrite(m_dirPin, m_direction);
}

// set the number of rotations per minute
void SimpleStepper::setRPM(float rpm){
 // we compute the lapse of time between 2 microsteps
 float rps = rpm/60.;

 m_intervaluSec = (float)1000000 / (rps * (float)m_nbSteps);
 m_intervalmSec = m_intervaluSec / 1000;
 m_intervalmSec = m_intervaluSec / 1000;
}

// run a microstep in accordance to the speed needed
void SimpleStepper::runOneMicrostep(){
  if(m_intervaluSec>m_LIMIT_OF_DELAY_USECONDS){
    runOneSlowMicrostep();
  }else{
    runOneQuickMicrostep();
  }
}

// private: run a microstep in accordance to the speed needed (in case of quite low speed)
void SimpleStepper::runOneSlowMicrostep(){
  digitalWrite(m_stepPin, HIGH);
  delay(m_intervalmSec/2);
  digitalWrite(m_stepPin, LOW);
  delay(m_intervalmSec/2);

}

// private: run a microstep in accordance to the speed needed (in case of quite fast speed)
void SimpleStepper::runOneQuickMicrostep(){
  digitalWrite(m_stepPin, HIGH);
  delayMicroseconds(m_intervaluSec/2);
  digitalWrite(m_stepPin, LOW);
  delayMicroseconds(m_intervaluSec/2);
}

void SimpleStepper::turn(){

}

// perform a specific number of microsteps  in accordance to the speed
void SimpleStepper::rotationMicrosteps(long int nbMicrosteps){

	long int nbMicrostepsTemp = nbMicrosteps;

	while(nbMicrostepsTemp){
		runOneMicrostep();
		nbMicrostepsTemp --;
	}

}

 // perform a specific number of steps in accordance to the speed
void SimpleStepper::rotationSteps(long int nbSteps){
	rotationMicrosteps(nbSteps * _MICROSTEP_);
}

// turn of a number of degrees
void SimpleStepper::rotationDegrees(float nbDegrees){
  long int nbStepForSuchAngle = (nbDegrees / 360.) * m_nbSteps;
  rotationMicrosteps(nbStepForSuchAngle);
}


 // perform a micro step, rather fast, not related with the speed computation from setRPM().
 // used in the need of triggering an independent micro step, but not suitable for constant revolution
void SimpleStepper::renegateMicroStep(){
  digitalWrite(m_stepPin, HIGH);
  delayMicroseconds(3);
  digitalWrite(m_stepPin, LOW);
  delayMicroseconds(3);
}

// performs _MICROSTEP_ renegate micro steps ( see renegateMicroStep() for further details)
void SimpleStepper::renegateStep(){
	for(int i=0; i<_MICROSTEP_; i++){
		renegateMicroStep();
	}
}


// performs _MICROSTEP_/2 renegate micro steps ( see renegateMicroStep() for further details)
void SimpleStepper::renegateHalfStep(){
	for(int i=0; i<_MICROSTEP_/2; i++){
		renegateMicroStep();
	}
}

// performs _MICROSTEP_/2 renegate micro steps ( see renegateMicroStep() for further details)
void SimpleStepper::renegateQuarterStep(){
	for(int i=0; i<_MICROSTEP_/4; i++){
		renegateMicroStep();
	}
}
