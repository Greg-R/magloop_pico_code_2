/* A "proof of concept" project to replace the STM32F103 "Blue Pill"
   which is used in the "Magnetic Loop Controller" described in the book
   "Microcontroller Projects for Amateur Radio by Jack Purdum, W8TEE, and
   Albert Peter, AC8GY" with the Raspberry Pi Pico.
   Copyright (C) 2022  Gregory Raven
      This is a derivative for the Raspberry Pi Pico SDK.
   It will not work with the Arduino IDE.

 * Rotary encoder handler for arduino.
 *
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 */

//#ifndef Rotary_h
//#define Rotary_h

#include "Arduino.h"
#include "pico/stdlib.h"

// Enable this to emit codes twice per step.
// #define HALF_STEP

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Counter-clockwise step.
#define DIR_CCW 0x20

class Rotary
{
public:
  Rotary(int _pin1, int _pin2);
  unsigned char process();
  void begin(bool internalPullup = true, bool flipLogicForPulldown = false);
  uint8_t result;

private:
  unsigned char state;
  unsigned char pin1;
  unsigned char pin2;
  unsigned char inverter;
};

//#endif
