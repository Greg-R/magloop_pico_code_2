/* A "proof of concept" project to replace the STM32F103 "Blue Pill"
   which is used in the "Magnetic Loop Controller" described in the book
   "Microcontroller Projects for Amateur Radio by Jack Purdum, W8TEE, and
   Albert Peter, AC8GY" with the Raspberry Pi Pico.
   Copyright (C) 2022  Gregory Raven

                                                    LICENSE AGREEMENT

  This program source code and its associated hardware design at subject to the GNU General Public License version 2,
                  https://opensource.org/licenses/GPL-2.0
  with the following additional conditions:
    1. Any commercial use of the hardware or software is prohibited without express, written, permission of the authors.
    2. This entire comment, unaltered, must appear at the top of the primary source file. In the Arduino IDE environemnt, this comment must
       appear at the top of the INO file that contains setup() and loop(). In any other environmentm, it must appear in the file containing
       main().
    3. This notice must appear in any derivative work, regardless of language used.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    A copy of the GPL-2.0 license is included in the repository as file LICENSE.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#pragma once
#include <stdint.h>
#include "pico/stdlib.h"
#include <string>
#include <vector>
#include <utility>
#include "hardware/spi.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "Adafruit_ILI9341.h"
//#include "DisplayManagement.h"
#include "AccelStepper.h"
#include "StepperManagement.h"
#include "DDS.h"
#include "SWR.h"
#include "EEPROM.h"
#include "Data.h"
#include "Button.h"
//#include "TuneInputs.h"
#include "TmcStepper.h"
#include "DisplayUtility.h"
#include "FreeSerif9pt7b.h"
#include "FreeSerif12pt7b.h"
#include "FreeSerif24pt7b.h"
#include "FreeMono9pt7b.h"
#include "FreeMono12pt7b.h"
#include "FreeMono24pt7b.h"

//int menuEncoderMovement;
//int frequencyEncoderMovement;
//int frequencyEncoderMovement2;
//int digitEncoderMovement;

//  This class is intended to manage various frequency and position related constants and variables.
//  The single object will be referenced by most or maybe all of the other class objects.

class Hardware  : public DisplayUtility
{

public:

Adafruit_ILI9341 &tft;
DDS &dds;
SWR &swr;
Button &enterbutton;
Button &autotunebutton;
Button &exitbutton;
Data &data;
TmcStepper &tmcstepper;
StepperManagement &stepper;
const int titleCoorX = 10;
const int titleCoorY = 37;
const int dataCoorX = 10;
const int dataCoorY = 75;

  // Flags used to indicate switch closures.
  bool maxclose;
  bool zeroclose;

  // Bands used:

  std::vector<std::string> tests = {"Button Test", "Encoder Test", "SWR Test", "Motor Test", "Confirm Hardware"};

  //  This should be made variable length arrays.
  //float countPerHertz[3];
  //float hertzPerStepperUnitVVC[3]; // Voltage Variable Cap

  // GPIO assignments.
  //  Buttons
  //uint enterButton = 6;
  //uint autotuneButton = 7;
  //uint exitButton = 9;
  // Zero and Maximum switches.
  //uint zeroswitch = 10;
  //uint maxswitch = 11;
  // Stepper position.  This is here because it is not always convenient to interogate the stepper object.
  int32_t position;
    enum class State
    {
        state0,
        state1,
        state2,
        state3
    }; // Used to move between states in state machines.
    State state;
    volatile int menuEncoderState;
    int submenuIndex, pauseTime;
    //std::array<std::string, 5> parameterNames;

  Hardware(Adafruit_ILI9341 &tft, DDS &dds, SWR &swr, Button &enterbutton, Button &autotunebutton, Button &exitbutton, Data &data, StepperManagement &stepper, TmcStepper &tmcstepper);

  int32_t UserNumericInput2(Button buttonAccept, Button buttonReject, int32_t number);
  
  void SWR_Test();

  void ButtonTest();

  void EncoderTest();

  void MotorTest();

  void SelectTest();

  void InitialTests();

  void RestorePreviousChoice(int submenuIndex);

  void HighlightNextChoice(int submenuIndex);

  void EraseTitle();

};