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

#include "TuneInputs.h"

TuneInputs::TuneInputs(Adafruit_ILI9341 &tft,
                               EEPROMClass &eeprom, Data &data, DDS& dds, Button &enterbutton, Button &autotunebutton, Button &exitbutton, TmcStepper &tmcstepper)
                                : DisplayUtility(tft, dds, swr, data, tmcstepper),
                                  tft(tft), eeprom(eeprom), data(data), enterbutton(enterbutton), autotunebutton(autotunebutton), exitbutton(exitbutton), tmcstepper(tmcstepper)
{
  parameters[0] = data.workingData.zero_offset;
  parameters[1] = data.workingData.backlash;
  parameters[2] = data.workingData.coarse_sweep;
  parameters[3] = data.workingData.accel;
  parameters[4] = data.workingData.speed;
  parameterNames = {"Zero Offset", "Backlash", "Coarse Sweep", "Acceleration", "Speed"};
  submenuIndex = 0;
}


// Used in Hardware Settings to select a parameter.
// If the Enter button is pressed, the hardware parameter can be changed and saved.
// Exit saves and moves back up to the Calibrate menu.
void TuneInputs::SelectParameter()
{
  int frequency;
  bool lastexitbutton = true;
  bool lastenterbutton = true;
  int32_t parameter;
  
  menuEncoderState = 0;
  state = State::state0; // Enter state0 which does graphics.
  tft.fillScreen(ILI9341_BLACK);
  //  Preset state selection machine
  while (true)
  {
    // Poll 2 buttons:
    enterbutton.buttonPushed();
    exitbutton.buttonPushed();

    switch (state)
    {
    case State::state0: // This state does the graphics.
      updateMessageTop("Menu Encoder to select, Enter to Adjust");
      EraseBelowMenu();
      tft.setTextSize(1); 
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); 
      // Show current hardware parameters
      for (int i = 0; i < 5; i++)
      {
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        tft.setFont(&FreeSerif12pt7b);
        tft.setCursor(30, 70 + i * 30);
        tft.print(i + 1);
        tft.print(".");
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        tft.setCursor(65, 70 + i * 30);
        tft.print(parameterNames[i].c_str());
        tft.setCursor(215, 70 + i * 30);
        tft.setFont(&FreeMono12pt7b);
        tft.print(parameters[i]);
      }
      tft.setTextColor(ILI9341_MAGENTA, ILI9341_WHITE);
      tft.setCursor(215, 70 + submenuIndex * 30);
      tft.print(parameters[submenuIndex]);  // Highlight selection.
      state = State::state1;
      break;

    case State::state1: // This state reads the encoders and button pushes.
    menuEncoderPoll();
      if (menuEncoderMovement == 1)
      { // Turning clockwise
        RestorePreviousChoice(submenuIndex);
        submenuIndex++;
        if (submenuIndex > 4)
          submenuIndex = 0;
        HighlightNextChoice(submenuIndex);
        menuEncoderMovement = 0;
      }
      if (menuEncoderMovement == -1)
      { // Tuning counter-clockwise
        RestorePreviousChoice(submenuIndex);
        submenuIndex--;
        if (submenuIndex < 0)
          submenuIndex = 4;
        HighlightNextChoice(submenuIndex);
        menuEncoderMovement = 0;
      }
      // Go to the chosen parameter and change it.
      if (enterbutton.pushed & not lastenterbutton)
      {
        // This switch was necessary because it was not convenient to put the parameters in an array.
        // It might be possible to use some form of C++ pointer array instead.
        switch(submenuIndex)
        {
          case 0:
            parameter = data.workingData.zero_offset;
            parameter = ChangeParameter(parameter);
            data.workingData.zero_offset = parameter;
            parameters[0] = parameter;
            break;
          case 1:
            parameter = data.workingData.backlash;
            parameter = ChangeParameter(parameter);
            data.workingData.backlash = parameter;
            parameters[1] = parameter;
            break;
          case 2:
            parameter = data.workingData.coarse_sweep;
            parameter = ChangeParameter(parameter);
            data.workingData.coarse_sweep = parameter;
            parameters[2] = parameter;
            break;
          case 3:
            parameter = data.workingData.accel;
            parameter = ChangeParameter(parameter);
            data.workingData.accel = parameter;
            parameters[3] = parameter;
            break;
          case 4:
            parameter = data.workingData.speed;
            parameter = ChangeParameter(parameter);
            data.workingData.speed = parameter;
            parameters[4] = parameter;
            break;
          default:
          break;
        }
        lastexitbutton = true;  // Prevents exit button from skipping a level.
        eeprom.put(0, data.workingData);  // Save parameters to EEPROM.
        eeprom.commit();
        //  Need to refresh graphics, because they were changed by ChangeFrequency!
        state = State::state0; // Refresh the graphics.
      }
      lastenterbutton = enterbutton.pushed;

      break;
    default:
      break;
    } // end switch of state machine
      // Process button pushes after switch statement, but before end of while block.
    if (exitbutton.pushed & not lastexitbutton)
      break; // Exit preset select, return the frequency and proceed to AutoTune.
    lastexitbutton = exitbutton.pushed;

  }   // end while SelectParameter state selection machine
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
int32_t TuneInputs::ChangeParameter(int32_t frequency) // Al Mod 9-8-19
{
  int32_t i, changeDigit, digitSpacing, halfScreen, incrementPad, insetMargin, insetPad, offset;
  int32_t defaultIncrement = 1;
  int32_t cursorOffset = 0;
  insetPad = 57; // Used to align digit indicator
  incrementPad = 05;
  digitSpacing = 28;
  insetMargin = 15;
  defaultIncrement = 1;
  halfScreen = data.PIXELHEIGHT / 2 - 25;
  bool lastexitbutton = true;
  bool lastenterbutton = true;
  digitEncoderMovement = 0;
  menuEncoderMovement = 0;
  updateMessageTop("          Enter New Hardware Parameter");
  tft.drawFastHLine(0, 20, 320, ILI9341_RED);
  //  The following configures the display for parameter selection mode.
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
  // End of custom code for this function.

return UserNumericInput(exitbutton, exitbutton, frequency);
}

// These functions are slightly different than the ones in the DisplayUtility class.
void TuneInputs::RestorePreviousChoice(int submenuIndex)
{
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); // restore old background
  tft.setCursor(215, 70 + submenuIndex * 30);
  tft.print(parameters[submenuIndex]);
}

void TuneInputs::HighlightNextChoice(int submenuIndex)
{
  tft.setTextColor(ILI9341_MAGENTA, ILI9341_WHITE); // HIghlight new preset choice
  tft.setCursor(215, 70 + submenuIndex * 30);
  tft.print(parameters[submenuIndex]);
}
