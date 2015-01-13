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


#include <Arduino.h>


/*
  JOSTICK class manages the joystick as sold by Sparkfun,
  with 2 axis and pressable.
  Both axis acts like a potentiometer (0 to 1023 on Arduino)
  but need to be scaled at the begining because idle position
  is not 512.
*/

class Joystick{

private:

  // a tolerance on reading horizontal and vertical values
  // computed in calibrates(), depends on idle positionning
  float _tolerance;

  // PINS
  int _verticalPin;  // analog
  int _horizontalPin;  // analog
  int _selectorPin;  // digital

  // Idle values are useful because the idle position
  // is not 512 on a joystick, it has to be scaled
  int _idleHorizontal;
  int _idleVertical;

  // used for measuring a short or long press on the stick
  long _chronoStart;
  long _chronoStop;

  // threshold in ms for long press on the stick
  long _pressTimeThreshold;

  // reads the raw horizontal value (0 to 1023)
  int readsRawHorizontal();

  // reads the raw vertical value (0 to 1023)
  int readsRawVertical();

  // calibrates the joystick to correct the idle values
  void calibrates();

public:
  // constructor, takes pins
  Joystick(int selectorPin, int horizontalPin, int verticalPin);

  // return horizontal normalized value in [-1, 1]
  // and 0 is considered idle
  float getNormalizedHorizontal();

  // return vertical normalized value in [-1, 1]
  // and 0 is considered idle
  float getNormalizedVertical();

  // returns true when pressed
  boolean isPressed();

  // start the time reccord for selector pressing
  void startReccordTime();

  // stop the time reccord for selector pressing
  // returns true if over the threshold
  bool stopReccordTime();

  // just a getter
  long getPressTimeThreshold();

  // just a setter
  void setPressTimeThreshold(long t);
};
