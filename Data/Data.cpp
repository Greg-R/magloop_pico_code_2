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

#include "Data.h"

Data::Data()
{
   maxclose = false;
   zeroclose = false;
};

void Data::computeSlopes()
{
   countPerHertz[0] = ((float)workingData.bandLimitPositionCounts[0][1] - (float)workingData.bandLimitPositionCounts[0][0]) / ((float)HIGHEND40M - (float)LOWEND40M);
   countPerHertz[1] = ((float)workingData.bandLimitPositionCounts[1][1] - (float)workingData.bandLimitPositionCounts[1][0]) / ((float)HIGHEND30M - (float)LOWEND30M);
   countPerHertz[2] = ((float)workingData.bandLimitPositionCounts[2][1] - (float)workingData.bandLimitPositionCounts[2][0]) / ((float)HIGHEND20M - (float)LOWEND20M);
   hertzPerStepperUnitVVC[0] = ((float)HIGHEND40M - (float)LOWEND40M) / ((float)workingData.bandLimitPositionCounts[0][1] - (float)workingData.bandLimitPositionCounts[0][0]);
   hertzPerStepperUnitVVC[1] = ((float)HIGHEND30M - (float)LOWEND30M) / ((float)workingData.bandLimitPositionCounts[1][1] - (float)workingData.bandLimitPositionCounts[1][0]);
   hertzPerStepperUnitVVC[2] = ((float)HIGHEND20M - (float)LOWEND20M) / ((float)workingData.bandLimitPositionCounts[2][1] - (float)workingData.bandLimitPositionCounts[2][0]);
}

void Data::writeDefaultValues()
{
   workingData.presetFrequencies[0][0] = 7030000L;
   workingData.presetFrequencies[0][1] = 7040000L;
   workingData.presetFrequencies[0][2] = 7100000L;
   workingData.presetFrequencies[0][3] = 7150000L;
   workingData.presetFrequencies[0][4] = 7250000L;
   workingData.presetFrequencies[0][5] = 7285000L;

   workingData.presetFrequencies[1][0] = 10106000L;
   workingData.presetFrequencies[1][1] = 10116000L;
   workingData.presetFrequencies[1][2] = 10120000L;
   workingData.presetFrequencies[1][3] = 10130000L;
   workingData.presetFrequencies[1][4] = 10140000L;
   workingData.presetFrequencies[1][5] = 10145000L;

   workingData.presetFrequencies[2][0] = 14030000L;
   workingData.presetFrequencies[2][1] = 14060000L;
   workingData.presetFrequencies[2][2] = 14100000L;
   workingData.presetFrequencies[2][3] = 14200000L;
   workingData.presetFrequencies[2][4] = 14250000L;
   workingData.presetFrequencies[2][5] = 14285000L;

   workingData.bandLimitPositionCounts[0][0] = 0;
   workingData.bandLimitPositionCounts[0][1] = 0;
   workingData.bandLimitPositionCounts[1][0] = 0;
   workingData.bandLimitPositionCounts[1][1] = 0;
   workingData.bandLimitPositionCounts[2][0] = 0;
   workingData.bandLimitPositionCounts[2][1] = 0;

   workingData.bandEdges[0][0] = LOWEND40M;
   workingData.bandEdges[0][1] = HIGHEND40M;
   workingData.bandEdges[1][0] = LOWEND30M;
   workingData.bandEdges[1][1] = HIGHEND30M;
   workingData.bandEdges[2][0] = LOWEND20M;
   workingData.bandEdges[2][1] = HIGHEND20M;

   workingData.currentBand = 0;
   workingData.currentFrequency = 7150000;
   workingData.initialized = 0x55555555;
   workingData.calibrated = 0;

   workingData.zero_offset = 1000;  // zero offset
   workingData.backlash = 60;    // backlash
   workingData.coarse_sweep = 20;    // coarse tune
   workingData.accel = 2000;  // acceleration
   workingData.speed = 500;   // speed
}
