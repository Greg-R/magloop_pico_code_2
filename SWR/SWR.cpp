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

#include "SWR.h"
//#include <Arduino.h>


SWR::SWR()
{
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);
  forward_offset = 0;
  reverse_offset = 0;
}

/*
Read and store the ADC offset values.  This must be done with the DDS off.
Note that the third ADC input is grounded on the board; this is used to
compensate for inherent ADC input offset.
*/
void SWR::ReadADCoffsets()
{
  busy_wait_ms(1000);
  adc_select_input(2);
  ground_offset = adc_read(); // Read the shorted input.
  busy_wait_ms(1000);
  adc_select_input(0);
  reverse_offset = adc_read() - ground_offset; // Subtract the ground offset.
  busy_wait_ms(1000);
  adc_select_input(1);
  forward_offset = adc_read() - ground_offset;
  return;
}

/*****
  Purpose: To read one bridge measurement

  Parameter list:
  void

  Return value:
  int           the swr * 1000 so it comes back as an int

  CAUTION: Assumes that frequency has already been set
  ALTERNATIVE CALCULATION (Untested ?)
    p = sqrt( ((float)REV) / FWD );   // Calculate reflection coefficient
    VSWR = (1 + p) / (1 - p);         // Calculate VSWR
*****/
float SWR::ReadSWRValue()
{
  int i;
  float sum[2] = {0.0, 0.0};

  float FWD = 0.0;
  float REV = 0.0;
  float VSWR;
  for (i = 0; i < MAXPOINTSPERSAMPLE; i++)
  { // Take multiple samples at each frequency
    busy_wait_ms(20);
    adc_select_input(1);
    sum[0] += (float)(adc_read() - ground_offset); // - (float) forward_offset;  // Read forward voltage, subtract ground offset.
    busy_wait_ms(20);
    adc_select_input(0);
    sum[1] += (float)(adc_read() - ground_offset); // - (float) reverse_offset;  // Read reverse voltage, subtract ground offset.
  }
  forward_voltage = sum[0] / (float)MAXPOINTSPERSAMPLE - (float)forward_offset;
  reverse_voltage = sum[1] / (float)MAXPOINTSPERSAMPLE - (float)reverse_offset;
  // REV = REV+SWRREVOFFSET;
  if (reverse_voltage >= forward_voltage)
  {
    VSWR = 999.0; // To avoid a divide by zero or negative VSWR then set to max 999
  }
  else
  {
    VSWR = ((forward_voltage + reverse_voltage) / (forward_voltage - reverse_voltage)); // Calculate VSWR
  }
  static float sNow = 1000.000, sLast = 1000.000;
  if (VSWR < 999.0)
  {
    sNow = VSWR;
    if (sNow < sLast)
    {
      sLast = sNow;
    }
  }

  return VSWR;
}

/*****
  Purpose: To ReadNewSWRValue.  What is different???
  Parameter list:
  void
  return (VSWR)

*****/
float SWR::ReadNewSWRValue()
{
  int i;
  int sum[2] = {0, 0};
  int FWD = 0;
  int REV = 0;
  float VSWR;
  // Needs to be updated to Pi Pico:
  //  for (i = 0; i < MAXPOINTSPERSAMPLE; i++) {             // Take multiple samples at each frequency
  //    sum[0] += analogRead(ANALOGFORWARD);
  //   sum[1] += analogRead(ANALOGREFLECTED);
  //  }
  FWD = sum[0] / MAXPOINTSPERSAMPLE;
  REV = sum[1] / MAXPOINTSPERSAMPLE;

  if (REV >= FWD)
  {
    VSWR = 999.0; // To avoid a divide by zero or negative VSWR then set to max 999
  }
  else
  {
    VSWR = ((float)(FWD + REV)) / ((float)(FWD - REV)); // Calculate VSWR
  }

  return (VSWR);
}
