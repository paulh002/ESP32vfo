// -----
// RotaryEncoder.h - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
//
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
//
// This work is licensed under a BSD 3-Clause style license,
// https://www.mathertel.de/License.aspx.
//
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// 16.06.2019 pin initialization using INPUT_PULLUP
// 10.11.2020 Added the ability to obtain the encoder RPM
// 29.01.2021 Options for using rotary encoders with 2 state changes per latch.
// -----

#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "Arduino.h"
#include "Wire.h"

const int max_enc = 4;

class RotaryEncoder
{
public:
  enum class Direction {
    NOROTATION = 0,
    CLOCKWISE = 1,
    COUNTERCLOCKWISE = -1
  };

  enum class LatchMode {
    FOUR3 = 1, // 4 steps, Latch at position 3 only (compatible to older versions)
    FOUR0 = 2, // 4 steps, Latch at position 0 (reverse wirings)
    TWO03 = 3  // 2 steps, Latch at position 0 and 3 
  };

  // ----- Constructor -----
  RotaryEncoder(uint8_t address, uint8_t cnt = 2, LatchMode mode = LatchMode::FOUR0, TwoWire* wire = &Wire);

  // retrieve the current position
  long getPosition(int enc);

  // simple retrieve of the direction the knob was rotated last time. 0 = No rotation, 1 = Clockwise, -1 = Counter Clockwise
  Direction getDirection(int enc);

  // adjust the current position
  void setPosition(int enc, long newPosition);

  // call this function every some milliseconds or by using an interrupt for handling state changes of the rotary encoder.
  void tick(void);

  // Returns the time in milliseconds between the current observed
  unsigned long getMillisBetweenRotations(int enc) const;

  // Returns the RPM
  unsigned long getRPM(int enc);
  bool begin(uint8_t dataPin, uint8_t clockPin, uint16_t freq);
  void	  setButtonPorts(uint8_t b1, uint8_t b2);
  uint8_t IsButtonPressed(uint8_t b1);
  bool    isConnected();
  void 	  ClearEncoder(int enc);

private:
  uint8_t   _address; // PCF8574 address
  TwoWire*  _wire;
  LatchMode _mode; // Latch mode from initialization
  uint8_t	_buttonports[2] = { 255, 255 };
  bool		_pressed[2] = { false, false };
  uint8_t   _cnt;
  
  volatile  int8_t _oldState[max_enc];

  volatile  long _position[max_enc];        // Internal position (4 times _positionExt)
  volatile  long _positionExt[max_enc];     // External position
  volatile  long _positionExtPrev[max_enc]; // External position (used only for direction checking)

  unsigned  long _positionExtTime[max_enc];     // The time the last position change was detected.
  unsigned  long _positionExtTimePrev[max_enc]; // The time the previous position change was detected.

  uint8_t   _read8();
 
};

#endif

// End