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
#include <string>
#include <utility>
#include <array>
#include "pico/stdlib.h"
#include "Adafruit_ILI9341.h"
#include "Arduino.h"
#include "DDS.h"
#include "SWR.h"
#include "StepperManagement.h"
#include "EEPROM.h"
#include "Data.h"
#include "Button.h"
#include "DisplayUtility.h"
#include "FreeSerif9pt7b.h"
#include "FreeSerif12pt7b.h"
#include "FreeSerif24pt7b.h"
#include "FreeMono9pt7b.h"
#include "FreeMono12pt7b.h"
#include "FreeMono24pt7b.h"


//  TuneInputs inherits from class DisplayUtility.
class TuneInputs : public DisplayUtility {

public:
    Adafruit_ILI9341 &tft;
    EEPROMClass &eeprom;
    Data &data;
    Button &enterbutton;
    Button &autotunebutton;
    Button &exitbutton;
    int whichBandOption;  // This indicates the current band in use.
    float SWRValue;
    float SWRcurrent;
    float readSWRValue;
    int position;
    int menuIndex;
    int submenuIndex;
    volatile int menuEncoderState;
    std::array<std::string, 5> parameterNames;
    std::array<int32_t, 5> parameters; //={data.workingData.zero_offset, data.workingData.backlash, data.workingData.coarse_sweep, data.workingData.accel, data.workingData.speed};
    std::array<int32_t, 5> hardware;
    enum class State
    {
        state0,
        state1,
        state2,
        state3
    }; // Used to move between states in state machines.
    State state;

    TuneInputs(Adafruit_ILI9341 &tft, EEPROMClass &eeprom, Data &data, DDS& dds, Button &enterbutton, Button &autotunebutton, Button &exitbutton);

    void SelectParameter();

    int32_t ChangeParameter(int32_t frequency);

    void RestorePreviousChoice(int submenuIndex);

    void HighlightNextChoice(int submenuIndex);

};