// -----
// RotaryEncoder.cpp - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
//
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
//
// This work is licensed under a BSD 3-Clause style license,
// https://www.mathertel.de/License.aspx.
//
// More information on: http://www.mathertel.de/Arduino
// -----
// Changelog: see RotaryEncoder.h
// -----

#include "RotaryEncoder.h"
#include "Arduino.h"

#define LATCH0 0 // input state at position 0
#define LATCH3 3 // input state at position 3


// The array holds the values �1 for the entries where a position was decremented,
// a 1 for the entries where the position was incremented
// and 0 in all the other (no change or not valid) cases.

const int8_t KNOBDIR[] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0};


// positions: [3] 1 0 2 [3] 1 0 2 [3]
// [3] is the positions where my rotary switch detends
// ==> right, count up
// <== left,  count down


// ----- Initialization and Default Values -----

RotaryEncoder::RotaryEncoder(uint8_t address, uint8_t cnt, LatchMode mode, TwoWire* wire)
{
  // Remember Hardware Setup
  _address = address;
  _mode = mode;
  _cnt = cnt;

  // Setup wire
  _wire = wire;
  for (int i = 0; i < max_enc; i++)
  {
      // start with position 0;
      // when not started in motion, the current state of the encoder should be 3
      _oldState[i] = 0x03;
      _position[i] = 0;
      _positionExt[i] = 0;
      _positionExtPrev[i] = 0;
  }
} // RotaryEncoder()

bool RotaryEncoder::begin(uint8_t dataPin, uint8_t clockPin, uint16_t freq)
{
    _wire = &Wire;
    if ((dataPin < 255) && (clockPin < 255))
    {
        _wire->begin(dataPin, clockPin, freq);
    }
    else {
        _wire->begin();
    }
    if (!isConnected()) return false;
    return true;
}

long RotaryEncoder::getPosition(int enc)
{
    if (enc >= 0 && enc < _cnt)
        return _positionExt[enc];
    return -1;
} // getPosition()

void RotaryEncoder::ClearEncoder(int enc)
{
    if (enc >= 0 && enc < _cnt)
	{
	  _oldState[enc] = 0x03;
      _position[enc] = 0;
      _positionExt[enc] = 0;
      _positionExtPrev[enc] = 0;
	}
    return;
} // getPosition()

RotaryEncoder::Direction RotaryEncoder::getDirection(int enc)
{
  RotaryEncoder::Direction ret = Direction::NOROTATION;
  if (enc >= 0 && enc < _cnt)
  {
      if (_positionExtPrev[enc] > _positionExt[enc]) {
          ret = Direction::COUNTERCLOCKWISE;
          _positionExtPrev[enc] = _positionExt[enc];
      }
      else if (_positionExtPrev[enc] < _positionExt[enc]) {
          ret = Direction::CLOCKWISE;
          _positionExtPrev[enc] = _positionExt[enc];
      }
      else {
          ret = Direction::NOROTATION;
          _positionExtPrev[enc] = _positionExt[enc];
      }
  }
  return ret;
}


void RotaryEncoder::setPosition(int enc, long newPosition)
{
    if (enc >= 0 && enc < _cnt)
    {
        switch (_mode) {
        case LatchMode::FOUR3:
        case LatchMode::FOUR0:
            // only adjust the external part of the position.
            _position[enc] = ((newPosition << 2) | (_position[enc] & 0x03L));
            _positionExt[enc] = newPosition;
            _positionExtPrev[enc] = newPosition;
            break;

        case LatchMode::TWO03:
            // only adjust the external part of the position.
            _position[enc] = ((newPosition << 1) | (_position[enc] & 0x01L));
            _positionExt[enc] = newPosition;
            _positionExtPrev[enc] = newPosition;
            break;
        } // switch
    }
} // setPosition()


void RotaryEncoder::tick(void)
{
  uint8_t val = _read8();
  int8_t thisState;
  for (int i = 0; i < _cnt; i++)
  {
      switch (i)
      {
      case 0:
          thisState = (val & 0x01) | (val & 0x02);
          break;
      case 1:
          thisState = (val & 0x04) | (val & 0x08);
          thisState = thisState >> 2;
          break;
      case 2:
          thisState = (val & 0x10) | (val & 0x20);
          thisState = thisState >> 4;
          break;
      case 3:
          thisState = (val & 0x40) | (val & 0x80);
          thisState = thisState >> 6;
          break;
      }
      if (_oldState[i] != thisState) {
          _position[i] += KNOBDIR[thisState | (_oldState[i] << 2)];
          _oldState[i] = thisState;

          switch (_mode) {
          case LatchMode::FOUR3:
              if (thisState == LATCH3) {
                  // The hardware has 4 steps with a latch on the input state 3
                  _positionExt[i] = _position[i] >> 2;
                  _positionExtTimePrev[i] = _positionExtTime[i];
                  _positionExtTime[i] = millis();
              }
              break;

          case LatchMode::FOUR0:
              if (thisState == LATCH0) {
                  // The hardware has 4 steps with a latch on the input state 0
                  _positionExt[i] = _position[i] >> 2;
                  _positionExtTimePrev[i] = _positionExtTime[i];
                  _positionExtTime[i] = millis();
              }
              break;

          case LatchMode::TWO03:
              if ((thisState == LATCH0) || (thisState == LATCH3)) {
                  // The hardware has 2 steps with a latch on the input state 0 and 3
                  _positionExt[i] = _position[i] >> 1;
                  _positionExtTimePrev[i] = _positionExtTime[i];
                  _positionExtTime[i] = millis();
              }
              break;
          } // switch
      } // if
  } // for
} // tick()


unsigned long RotaryEncoder::getMillisBetweenRotations(int enc) const
{
  return (_positionExtTime[enc] - _positionExtTimePrev[enc]);
}

unsigned long RotaryEncoder::getRPM(int enc)
{
  // calculate max of difference in time between last position changes or last change and now.
  unsigned long timeBetweenLastPositions = _positionExtTime[enc] - _positionExtTimePrev[enc];
  unsigned long timeToLastPosition = millis() - _positionExtTime[enc];
  unsigned long t = max(timeBetweenLastPositions, timeToLastPosition);
  return 60000.0 / ((float)(t * 20));
}

uint8_t RotaryEncoder::_read8()
{
    _wire->requestFrom(_address, (uint8_t)1);
    uint8_t val = _wire->read();
    if (_buttonports[0] < 8)
    {
        if (val & (1 << _buttonports[0]))
            _pressed[0] = false;
        else
            _pressed[0] = true;
    }
    if (_buttonports[1] < 8)
    {
        if (val & (1 << _buttonports[1]))
            _pressed[1] = false;
        else
            _pressed[1] = true;
    }
    return  val & 0x0F;
}

bool RotaryEncoder::isConnected()
{
    _wire->beginTransmission(_address);
    return (_wire->endTransmission() == 0);
}

void RotaryEncoder::setButtonPorts(uint8_t b1, uint8_t b2)
{
    if (b1 >= 0 || b1 < 8)
        _buttonports[0] = b1;

    if (b2 >= 0 || b2 < 8)
        _buttonports[1] = b2;
}

uint8_t RotaryEncoder::IsButtonPressed(uint8_t b1)
{
	if (b1 == 0 )
		return _pressed[0];

	if (b1 > 0)
		return _pressed[1];
	return 0;
}

// End