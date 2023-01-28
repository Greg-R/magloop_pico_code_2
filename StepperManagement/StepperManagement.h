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
#include "AccelStepper.h"
#include "DisplayUtility.h"
#include "Button.h"
#include "Data.h"
#include "TmcStepper.h"

// This class inherits from AccelStepper, which is an Arduino library.
class StepperManagement : public AccelStepper, public DisplayUtility
{

public:
   int32_t rotation;
   float countPerHertz[3];
   float hertzPerStepperUnitAir[3];
   //Button& exitbutton;
   Data &data;

   //  This constructor duplicates the parameters of the AccelStepper constructor.
   //  The actual parameters when instantiating:  DRIVER, STEPPERPUL, STEPPERDIR

   StepperManagement(Adafruit_ILI9341 &tft, DDS &dds, SWR &swr, Data &data, AccelStepper::MotorInterfaceType interface, uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, bool enable = true);

   void MoveStepperToPosition(int32_t position);

   void ResetStepperToZero();

   long ConvertFrequencyToStepperCount(uint32_t presentFrequency);
};