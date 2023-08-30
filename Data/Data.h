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

//  This class is intended to manage various frequency and position related constants and variables.
//  The single object will be referenced by most or maybe all of the other class objects.

class Data
{

public:

const std::string version = "main";
const std::string releaseDate = "8-30-23";

  // Flags used to indicate switch closures.
  bool maxclose;
  bool zeroclose;

  //  These are fundamental size definitions used throughout the project.
  const int PRESETSPERBAND = 6; // Allow this many preset frequencies on each band
  const int MAXBANDS = 3;       // Can only process this many frequency bands
  const int MAXMENUES = 3;
  const int PIXELWIDTH = 320;   // Display pixels width.
  const int PIXELHEIGHT = 240;  // Display pixels height.

  // Bands used:

  std::vector<std::string> bands = {"40M", "30M", "20M"};
  static const uint32_t LOWEND40M = 7000000;
  static const uint32_t HIGHEND40M = 7300000;
  static const uint32_t LOWEND30M = 10100000;
  static const uint32_t HIGHEND30M = 10150000;
  static const uint32_t LOWEND20M = 14000000;
  static const uint32_t HIGHEND20M = 14350000;

  // Preset frequency constants in the dataStruct are initial defaults; these defaults are saved to the
  // EEPROM initially, but they can be overwritten later if the user desires.  The presets will always
  // be read from the EEPROM.
  struct dataStruct
  {
    uint32_t presetFrequencies[3][6] =
        {
            {7030000L, 7040000L, 7100000L, 7150000L, 7250000L, 7285000L},       // 40M
            {10106000L, 10116000L, 10120000L, 10130000L, 10140000L, 10145000L}, // 30M
            {14030000L, 14060000L, 14100000L, 14200000L, 14250000L, 14285000L}  // 20M
    };
    uint32_t bandLimitPositionCounts[3][2];
    uint32_t bandEdges[3][2]; // = { // Band edges in Hz
                              //   {LOWEND40M, HIGHEND40M},
                              //   {LOWEND30M, HIGHEND30M},
                              //   {LOWEND20M, HIGHEND20M}};
    uint32_t currentBand = 0;
    uint32_t currentFrequency = 7150000;
    uint32_t lastFreq[3];  // Used to remember the last autotune frequency.
    uint32_t initialized = 0x55555555;
    uint32_t calibrated; //  Please run Initial Calibration! if not set to correct value.
    uint32_t hardware;   //  0x55555555 means hardware is accepted.
  // The following are parameters which must be "tuned" to the particular mechanics in use.
  // Refer to the documentation for setting these values.
  int zero_offset = 0; // 600 for full step.  1000 for half-step.  270 for dummy with 1/16 step.
  int backlash = 0;
  int coarse_sweep = 1;
  int accel = 2000;
  int speed = 500;
  } workingData;

  //  This should be made variable length arrays.
  float countPerHertz[3];
  float hertzPerStepperUnitVVC[3]; // Voltage Variable Cap

  // GPIO assignments.
  //  Buttons
  uint enterButton = 6;
  uint autotuneButton = 7;
  uint exitButton = 9;
  // Zero and Maximum switches.
  uint zeroswitch = 10;
  uint maxswitch = 11;
  // Stepper position.  This is here because it is not always convenient to interogate the stepper object.
  int32_t position;
  //  Power controls
  const int STEPPERSLEEPNOT = 9;
  const int OPAMPPOWER = 3;
  const int RFAMPPOWER = 2;
  const int RFRELAYPOWER = 19;
  //  Interface for the DDS object.
  const int DDS_RST = 4;
  const int DDS_DATA = 5;
  const int DDS_FQ_UD = 12;
  const int WLCK = 22;

  Data();

  void computeSlopes();

  void writeDefaultValues();
};