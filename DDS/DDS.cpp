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

#include "DDS.h"

// The routines in this file are use to interface with the AD9850 DDS.

DDS::DDS(unsigned int DDS_RST, unsigned int DDS_DATA, unsigned int DDS_FQ_UD, unsigned int DDS_WCLK) : RESET(DDS_RST), DATA(DDS_DATA), FQ_UD(DDS_FQ_UD), WCLK(DDS_WCLK)
{
  gpio_set_function(RESET, GPIO_FUNC_SIO);
  gpio_set_function(DATA, GPIO_FUNC_SIO);
  gpio_set_function(FQ_UD, GPIO_FUNC_SIO);
  gpio_set_function(WCLK, GPIO_FUNC_SIO);
  gpio_set_dir(RESET, GPIO_OUT);
  gpio_set_dir(DATA, GPIO_OUT);
  gpio_set_dir(FQ_UD, GPIO_OUT);
  gpio_set_dir(WCLK, GPIO_OUT);
}

void DDS::DDSWakeUp()
{ // Initialize DDS
  gpio_put(RESET, 1);
  busy_wait_us_32(20000);
  gpio_put(RESET, 0);
  busy_wait_us_32(20000);
  gpio_put(WCLK, 1);
  busy_wait_us_32(20000);
  gpio_put(WCLK, 0);
  busy_wait_us_32(20000);
  gpio_put(FQ_UD, 1); // This puts the device into serial mode.
  busy_wait_us_32(20000);
  gpio_put(FQ_UD, 0);
  busy_wait_us_32(20000);
}

void DDS::outOne()
{
  busy_wait_us_32(2000);
  gpio_put(WCLK, 0);
  busy_wait_us_32(2000);
  gpio_put(DATA, 1);
  busy_wait_us_32(2000);
  gpio_put(WCLK, 1);
  busy_wait_us_32(2000);
  gpio_put(DATA, 0);
  busy_wait_us_32(2000);
}

void DDS::outZero()
{
  busy_wait_us_32(2000);
  gpio_put(WCLK, 0);
  busy_wait_us_32(2000);
  gpio_put(DATA, 0);
  busy_wait_us_32(2000);
  gpio_put(WCLK, 1);
  busy_wait_us_32(2000);
  gpio_put(WCLK, 0);
  busy_wait_us_32(2000);
}

void DDS::byte_out(unsigned char byte)
{
  int i;
  for (i = 0; i < 8; ++i)
  {
    if ((byte & 1) == 1)
      outOne();
    else
      outZero();
    byte = byte >> 1;
  }
}

void DDS::SendFrequency(long frequency)
{ // Set DDS frequency

  long freq = frequency * 4294967295L / 125000000L; // note 125 MHz clock on 9850  Can be used to calibrate individual DDS
  busy_wait_us_32(2000);
  for (int b = 0; b < 4; b++, freq >>= 8)
    byte_out(freq & 0xFF);
  byte_out(0x00); // Final control byte, all 0 for 9850 chip
  busy_wait_us_32(2000);
  gpio_put(FQ_UD, 1); // Done!  Should see output
  busy_wait_us_32(2000);
  gpio_put(FQ_UD, 0);
  // Store the frequency in the DDS object, unless it is 0 (deactivates DDS).
  if (frequency != 0)
    currentFrequency = frequency;
  busy_wait_ms(100); // Settling time 100 ms.
}
