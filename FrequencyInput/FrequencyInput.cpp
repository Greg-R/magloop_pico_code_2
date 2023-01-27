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

#include "FrequencyInput.h"

FrequencyInput::FrequencyInput(Adafruit_ILI9341 &tft,
                               Data &data, Button &enterbutton, Button &autotunebutton, Button &exitbutton)
                                : DisplayUtility(tft, dds, swr, data), tft(tft), data(data), enterbutton(enterbutton), autotunebutton(autotunebutton), exitbutton(exitbutton)
{
}


/*****
  Purpose: Set new frequency
  This needs to be re-written as a state machine.???
  Argument list:
    int bandIndex    Which of the three bands was selected?
    long frequency   The current frequency.

  Return value:
    The new frequency is returned.

  Dependencies:  DDS, SWR, Adafruit_ILI9341
*****/
int32_t FrequencyInput::ChangeFrequency(int bandIndex, int32_t frequency) // Al Mod 9-8-19
{
  int i, changeDigit, digitSpacing, halfScreen, incrementPad, insetMargin, insetPad;
  long defaultIncrement;
  insetPad = 57; // Used to align digit indicator
  incrementPad = 05;
  digitSpacing = 10;
  insetMargin = 20;
  defaultIncrement = 1000L;
  halfScreen = data.PIXELHEIGHT / 2 - 25;
  bool lastexitbutton = true;
  bool lastenterbutton = true;
  updateMessageTop("          Enter New Preset Frequency");
  tft.drawFastHLine(0, 20, 320, ILI9341_RED);
  if (bandIndex == 0)
  {                // 40M
    insetPad = 32; // smaller number, so less spacing to a given digit
  }
  //  The following configures the display for frequency selection mode.
  EraseBelowMenu();
  tft.setTextSize(1);
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextColor(ILI9341_WHITE); // Messages
  tft.setCursor(insetMargin, halfScreen + 60);
  tft.print("Increment:");
  tft.setCursor(insetMargin + 90, halfScreen + 60);
  tft.print("Menu Encoder");
  tft.setCursor(insetMargin, halfScreen + 80);
  tft.print("Digit:");
  tft.setCursor(insetMargin + 90, halfScreen + 80);
  tft.print("Frequency Encoder");
  tft.setCursor(insetMargin, halfScreen + 100);
  tft.setCursor(insetMargin + 90, halfScreen + 100);
  tft.setCursor(insetMargin, halfScreen + 120);
  tft.print("Exit:");
  tft.setCursor(insetMargin + 90, halfScreen + 120);
  tft.print("Exit Button");
  tft.setTextSize(1);
  tft.setFont(&FreeSerif24pt7b);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(insetMargin + (insetPad + incrementPad) + digitSpacing * 6 - 28, halfScreen + 5); // Assume 1KHz increment
  tft.print("_");                                                                                 // underline selected character position
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(insetMargin, halfScreen);
  tft.setTextSize(1);
  tft.setFont(&FreeSerif24pt7b);
  tft.print(frequency);
  tft.setFont(&FreeSerif24pt7b);

  // State Machine for frequency input with encoders.
  while (true)
  { // Update frequency until user pushes AutoTune button.
    // Poll autotunebutton and exitbutton.
   // enterbutton.buttonPushed();
    exitbutton.buttonPushed();
    if (exitbutton.pushed & not lastexitbutton) {
  //    lastexitbutton = enterbutton.pushed;
      break;  // Break out of the while loop.  Returns original frequency.
    }
    // Make sure there is a proper transition of the enter button.
      lastexitbutton = exitbutton.pushed;
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.setFont(&FreeSerif24pt7b);
    if (digitEncoderMovement == 1)
    { // Change frequency digit increment
      tft.fillRect(0, halfScreen + 6, data.PIXELWIDTH * .90, 20, ILI9341_BLACK);
      defaultIncrement /= 10;
      if (defaultIncrement < 1)
      { // Don't go too far right
        defaultIncrement = 1L;
      }
      incrementPad += INCREMENTPAD;
      if (defaultIncrement > 1000000L)
      {
        defaultIncrement = 1000000L;
      }
      if (incrementPad > INCREMENTPAD * 4)
      { // Don't overshoot or...
        incrementPad -= INCREMENTPAD;
      }
      tft.setCursor(insetMargin + (insetPad + incrementPad) + digitSpacing * 6 - 28, halfScreen + 5); // Assume 1KHz increment
      tft.print("_");
      digitEncoderMovement = 0;
    }
    else
    {
      if (digitEncoderMovement == -1)
      {
        tft.fillRect(0, halfScreen + 6, data.PIXELWIDTH * .90, 20, ILI9341_BLACK);
        defaultIncrement *= 10;
        if (defaultIncrement > 1000000)
        { // Don't go too far right
          defaultIncrement = 1000000L;
        }
        incrementPad -= INCREMENTPAD;
        if (incrementPad < -INCREMENTPAD * 3) // Don't undershoot either
          incrementPad += INCREMENTPAD;

        tft.setCursor(insetMargin + (insetPad + incrementPad) + digitSpacing * 6 - 28, halfScreen + 5); // Assume 1KHz increment
        tft.print("_");
        digitEncoderMovement = 0;
      }
    }
    tft.setTextColor(ILI9341_GREEN);
    digitEncoderMovement = 0;
    menuEncoderMovement = 0;
    if (frequencyEncoderMovement)
    { // Change digit value
      frequency += (long)(frequencyEncoderMovement * defaultIncrement);
     // position = stepper.ConvertFrequencyToStepperCount(frequency);
      tft.fillRect(insetMargin, halfScreen - 35, data.PIXELWIDTH * .80, 40, ILI9341_BLACK);
      tft.setCursor(insetMargin, halfScreen);
      tft.setTextSize(1);
      tft.setFont(&FreeSerif24pt7b);
      tft.print(frequency);
      frequencyEncoderMovement = 0L; // Reset encoder flag
    }
  }                   // end while loop
  tft.setTextSize(2); // Back to normal
  tft.setTextColor(ILI9341_WHITE);
  return frequency;
}
