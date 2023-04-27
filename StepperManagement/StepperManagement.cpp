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

#include "StepperManagement.h"

StepperManagement::StepperManagement(Adafruit_ILI9341 &tft, DDS &dds, SWR &swr, Data &data, AccelStepper::MotorInterfaceType interface, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, bool enable) :AccelStepper(interface, pin1, pin2),
                                     DisplayUtility(tft, dds, swr, data), data(data)
{
  // position = 2500;          // Default to approximately midrange.
  rotation = -1;
  setCurrentPosition(5000);                //
  setAcceleration(data.workingData.accel); // Acceleration needs to be set high, with maximum speed limit.
  setSpeed(data.workingData.speed);
  setMaxSpeed(data.workingData.speed);
}

// Revised version which includes ZEROSWITCH and MAXSWITCH detection.
// Note the parameter data.zero_offset.  This is set for the particular
// mechanics used and must be determined empirically.
// This parameter is located in the file Data.h.
void StepperManagement::MoveStepperToPosition(int32_t position)
{
  // For switch protection, need to know which direction the stepper will move,
  // positive or negative direction.  Switch "recovery" will move the opposite direction.
  int32_t temp;
  data.maxclose = false;
  data.zeroclose = false;
  temp = position - currentPosition();
  // if((position - currentPosition()) > 0) rotation = 1;  // true is positive rotation.
  if (temp > 0)
    rotation = 1;
  else
    rotation = -1;

  moveTo(position);

  while (distanceToGo() != 0)
  {
    run();
    data.position = currentPosition();  // Write the current stepper position to the data object.
    // This code handles switch closures, both for calibration (zero), and maximum switch faults.
    if ((gpio_get(data.maxswitch) == false) or (gpio_get(data.zeroswitch) == false))
    {
      // Set flag while switch is still closed.
      if (gpio_get(data.maxswitch) == false)
        data.maxclose = true;
      if (gpio_get(data.zeroswitch) == false)
        data.zeroclose = true;
      stop(); // Properly decelerate and stop the stepper.
      runToPosition();
      // Rotate away from switch.  Rotate in opposite direction.
      move(-1 * rotation * data.workingData.zero_offset); //  Move the stepper off the zero switch.
      runToPosition();
      data.position = currentPosition();  // Write the current stepper position to the data object.
      break; // Escape from loop.
    }
  }
     if(data.maxclose) {
     EraseBelowMenu();
     updateMessageMiddle("   Maximum switch was closed, correct      problem!!!");
     // Cut power to halt controller.
     PowerStepDdsCirRelay(false, 0, false, false);
     return;
     //busy_wait_ms(3000);
     //EraseBelowMenu();
     }
     if(data.zeroclose) {
     EraseBelowMenu();
     updateMessageMiddle("            Zero switch was closed");
     busy_wait_ms(3000);
     EraseBelowMenu();
     }     
}

// A method to set the 0 position of the stepper motor.
// The stepper is rotated towards the zero switch until it closes.
// The stepper is properly decelerated and stopped, and then
// moved away from the switch such that it is open plus enough
// distance to clear the leaf switch, so that there is no pressure
// applied to it.

void StepperManagement::ResetStepperToZero()
{
  updateMessageTop("                   Setting to Zero");
  // Approach the zero switch slowly.
  setMaxSpeed(data.workingData.speed / 2);
  MoveStepperToPosition(-100000);
  setCurrentPosition(0); //  The stepper is now calibrated!  This function sets speed to 0.
  data.position = 0;
  setSpeed(data.workingData.speed);
  setMaxSpeed(data.workingData.speed); // Put maximum speed back to normal.
}

/*****
  Purpose: Allow the user to change frequency and have the stepper automatically follow frequency change

  Parameter list:
    long presentFrequency     the present frequency of the DDS

  Return value:
    void

  CAUTION:

*****/
long StepperManagement::ConvertFrequencyToStepperCount(uint32_t presentFrequency)
{
  long count;
  switch (data.workingData.currentBand)
  {
  case 40: //   intercept                  + slopeCoefficient * newFrequency
    count = data.workingData.bandLimitPositionCounts[0][0] + (long)(countPerHertz[0] * ((float)(presentFrequency - data.LOWEND40M)));
    break;

  case 30:
    count = data.workingData.bandLimitPositionCounts[1][0] + (long)(countPerHertz[1] * ((float)(presentFrequency - data.LOWEND30M)));
    break;

  case 20:
    count = data.workingData.bandLimitPositionCounts[2][0] + (long)(countPerHertz[2] * ((float)(presentFrequency - data.LOWEND20M)));
    break;

  default:
    break;
  }
  // position = count;
  return count;
}
