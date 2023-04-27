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

// This is a collection of often-used Display manipulation methods which are intended to be a base class for other display manipulating classes.

#include "DisplayUtility.h"

DisplayUtility::DisplayUtility(Adafruit_ILI9341 &tft, DDS &dds, SWR &swr, Data &data) : tft(tft), dds(dds), swr(swr), data(data)
{
  startUpFlag = false;
  calFlag = false;
  // menuEncoder = Rotary(20, 18); // Swap if encoder works in wrong direction.
  // frequencyEncoder = Rotary(21, 17);
  menuEncoder.begin(true, false);
  frequencyEncoder.begin(true, false);
  menuEncoderMovement = 0;
  frequencyEncoderMovement = 0;
  frequencyEncoderMovement2 = 0;
  digitEncoderMovement = 0;
  // Construct the TmcStepper object.
  TmcStepper tmcstepper = TmcStepper();
  // Set main configuration.  Set the TMC driver off.
  uart_write_blocking(uart1, tmcstepper.getCommand(tmcstepper.forward), 8);
  // Set the tmcConfig variable to the desired step size.  Driver is off by default.
  //tmcstepper.tmcConfig = tmcstepper.stepsize128;
  //this->tmcstepper.tmcConfig = this->tmcstepper.stepsize256;
  //this->tmcstepper.tmcConfig = this->tmcstepper.stepsize128;
  //this->tmcstepper.tmcConfig = this->tmcstepper.stepsize064;
  this->tmcstepper.tmcConfig = this->tmcstepper.stepsize032;
  //this->tmcstepper.tmcConfig = this->tmcstepper.stepsize008;
  // Set the step size.
  uart_write_blocking(uart1, tmcstepper.getCommand(tmcstepper.tmcConfig), 8);
}

/*****
  Purpose: To erase the display below the top two menu lines
  Argument list:
    void
  Return value:
    void
*****/
void DisplayUtility::EraseBelowMenu() // al mod 9-8-19
{
  tft.fillRect(0, 46, 340, 231, ILI9341_BLACK);
  tft.drawFastHLine(0, 45, 320, ILI9341_RED);
}

/*****
  Purpose: To erase the display page
  Argument list:
    void
  Return value:
    void
*****/
void DisplayUtility::ErasePage()
{
  tft.fillScreen(ILI9341_BLACK);
}

/*****
  Purpose: To display the main menu page

  Argument list:
    int whichMenuPage         the currently displayed menu page

  Return value:
    void
*****/
void DisplayUtility::ShowMainDisplay(int whichMenuPage)
{
  int lastMenuPage = 0;
  int i;
  tft.setFont();
  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLACK);
  for (i = 0; i < 3; i++)
  {
    if (i == whichMenuPage)
    {
      tft.setTextColor(ILI9341_BLUE, ILI9341_WHITE);
    }
    else
    {
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    }
    tft.setCursor(i * 100, 0);
    tft.print(menuOptions[i].c_str());
  }
}

/*****
  Purpose: To display the SWR, frequency, and step data.
  An auxiliary function, Update frequency, does the frequency and the step.
  Argument list:
    float SWR, the current SWR value to be displayed
    int currentFrequency, the frequency to be displayed
  Return value:
    void
*****/
void DisplayUtility::ShowSubmenuData(float SWR, int currentFrequency) // al mod 9-8-19
{
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setFont(&FreeSerif9pt7b);
  tft.fillRect(0, 23, data.PIXELWIDTH, 20, ILI9341_BLACK);
  tft.drawFastHLine(0, 20, 320, ILI9341_RED);
  tft.setCursor(0, 40);
  tft.print("SWR ");
  tft.setFont(&FreeSerif12pt7b);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  if (SWR > 50.0 or SWR < .5)
  {                  // Real or bogus SWR?
    tft.print("??"); //...bogus
  }
  else
  {
    if (SWR > 9.9999)
    {
      tft.print(SWR, 2);
    }
    else
    {
      tft.print(SWR, 2); // real
    }
  }
  // UpdateFrequency(currentFrequency);
  tft.setTextSize(1);
  tft.setFont(&FreeSerif9pt7b);
  tft.fillRect(140, 25, data.PIXELWIDTH, 20, ILI9341_BLACK);
  tft.setCursor(100, 40);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.print("FREQ ");
  tft.setFont(&FreeSerif12pt7b);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.print(currentFrequency);
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.print("  p ");
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.print(data.position);

  tft.drawFastHLine(0, 45, 320, ILI9341_RED);
}

/*****
  Purpose: Update the SWR value
  Why not simply use a single function for SWR, frequency, and step???
  Argument list:
    float SWR                 the current SWR value

  Return value:
    void
*****/
void DisplayUtility::UpdateSWR(float SWR, std::string msg)
{
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(60, 30);
  if (msg.size() > 0)
  {
    tft.print(msg.c_str());
  }
  else
  {
    if (SWR > .5 && SWR < 50.0)
    {
      tft.print(SWR);
    }
    else
    {
      tft.print("> 50");
    }
  }
}

/*****
  Purpose: Update Top Message Area

  Argument list:
    char message

  Return value:
    void
*****/
void DisplayUtility::updateMessageTop(std::string messageToPrint)
{
  tft.fillRect(0, 0, 320, 20, ILI9341_BLACK); // Erase top line.
  tft.drawFastHLine(0, 20, 320, ILI9341_RED);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setFont(&FreeSerif9pt7b);
  tft.setCursor(10, 12);
  tft.print(messageToPrint.c_str());
}

/*****
  Purpose: Update Middle Message Area

  Argument list:
    char message

  Return value:
    void
*****/
void DisplayUtility::updateMessageMiddle(std::string messageToPrint)
{
  tft.fillRect(0, 100, 319, 140, ILI9341_BLACK); // Erase top line.
  // tft.drawFastHLine(0, 100, 319, ILI9341_RED);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setFont(&FreeSerif9pt7b);
  tft.setCursor(10, 120);
  tft.print(messageToPrint.c_str());
}

/*****
  Purpose: Update Bottom Message Area

  Argument list:
    char message

  Return value:
    void
*****/
void DisplayUtility::updateMessageBottom(std::string messageToPrint)
{
  tft.fillRect(0, 200, 319, 240, ILI9341_BLACK); // Erase previous message.
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setFont(&FreeSerif9pt7b);
  tft.setCursor(10, 220);
  tft.print(messageToPrint.c_str());
}

/*****
  Purpose: To restore most recently highlighted preset choice

  Parameter list:
    Adafruit_ILI9341 tft      the display object
    int submenuIndex
    int whichBandOption

  Return value:
    void
*****/
void DisplayUtility::RestorePreviousPresetChoice(int submenuIndex, int whichBandOption)
{
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); // restore old background
  tft.setCursor(65, 70 + submenuIndex * 30);
  tft.print(data.workingData.presetFrequencies[whichBandOption][submenuIndex]);
}

/*****
  Purpose: To highlight new preset choice

  Parameter list:
    Adafruit_ILI9341 tft      the display object
    int submenuIndex
    int whichBandOption

  Return value:
    void
*****/
void DisplayUtility::HighlightNewPresetChoice(int submenuIndex, int whichBandOption)
{
  tft.setTextColor(ILI9341_MAGENTA, ILI9341_WHITE); // HIghlight new preset choice
  tft.setCursor(65, 70 + submenuIndex * 30);
  tft.print(data.workingData.presetFrequencies[whichBandOption][submenuIndex]);
}

/*****
  Purpose: Detect state change of data.maxswitch.  Warn the user.
           Back the stepper off the switch so that it goes back to normal state.

  Parameter list:


  Return value:
    int

  CAUTION:

*****/
int DisplayUtility::DetectMaxSwitch()
{
  if (gpio_get(data.maxswitch) == LOW)
  {
    //  stepper.move(-300);
    //  stepper.runToPosition();
    for (int i = 0; i < 10; i++)
    {
      updateMessageTop("                  Upper Limit Hit!");
      busy_wait_ms(1000);
      tft.fillRect(90, 0, 300, 20, ILI9341_BLACK);
      busy_wait_ms(1000);
    }
    return 1;
  }
  return 0;
}

/*
This method accepts numeric input from the user via the Menu and Frequency encoders.
This is intended for generic input of integers.
Note the two button parameters; buttonAccept enters and saves the number to EEPROM.
buttonReject bails out of the while loop.
If the two buttons are the same, the 2nd one (buttonReject) is ignored.
This is useful in some contexts where you want to always accept the entry.
*/

int32_t DisplayUtility::UserNumericInput(Button buttonAccept, Button buttonReject, int32_t number)
{
  int32_t i, changeDigit, digitSpacing, halfScreen, incrementPad, insetMargin, insetPad, offset, cursorHome;
  int32_t defaultIncrement = 1;
  int32_t cursorOffset = 0;
  uint32_t numberSize;
  insetPad = 57; // Used to align digit indicator
  incrementPad = 05;
  digitSpacing = 28;
  insetMargin = 15;
  defaultIncrement = 1;
  halfScreen = data.PIXELHEIGHT / 2 - 25;
  // bool lastexitbutton = true;
  // bool lastenterbutton = true;
  // bool lastautotunebutton = true;
  bool lastAcceptButton = true;
  bool lastRejectButton = true;
  digitEncoderMovement = 0;
  menuEncoderMovement = 0;

  // Determine the size of the number:
  numberSize = std::to_string(number).size();
  offset = 10 - numberSize;

  // First, print the number and the cursor (underscore) to the display.
  tft.setTextSize(1);
  tft.setFont(&FreeMono24pt7b);
  tft.setTextColor(ILI9341_WHITE);
  // Initially place cursor under single digit.
  cursorHome = 10 + digitSpacing * offset + digitSpacing * (numberSize - 1); // The initial placement of the cursor.
  tft.setCursor(cursorHome, halfScreen + 5);                                 // Assume 1KHz increment
  tft.print("_");                                                            // underline selected character position
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(10 + digitSpacing * offset, halfScreen);
  tft.setTextSize(1);
  tft.setFont(&FreeMono24pt7b);
  tft.print(number);
  // State Machine for frequency input with encoders.
  while (true)
  { // Update number until user pushes button.
    menuEncoderPoll();
    freqEncoderPoll();
    // Poll accept button.
    buttonAccept.buttonPushed();
    if (buttonAccept.pushed & not lastAcceptButton)
    {
      break; // Break out of the while loop.
    }
    lastAcceptButton = buttonAccept.pushed;

    // Don't look at the reject button if it is the same as the accept.
    if (buttonReject.gpio != buttonAccept.gpio)
    {
      buttonReject.buttonPushed();
      if (buttonReject.pushed & not lastRejectButton)
      {
        return number = 0; // Exit, don't change, returning 0 means exit.
      }
      lastRejectButton = buttonReject.pushed;
    }

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.setFont(&FreeMono24pt7b);
    // Handle movement of the cursor to the right.
    if (digitEncoderMovement == 1)
    {                                                                            // Change frequency digit increment
      tft.fillRect(0, halfScreen + 6, data.PIXELWIDTH * .91, 20, ILI9341_BLACK); // Erase existing cursor.
      defaultIncrement = defaultIncrement / 10;                                  // Move to right, so divide by 10.
      if (defaultIncrement < 1)
      { // Don't go too far right, reset defaultIncrement.
        defaultIncrement = 1;
      }
      else
        cursorOffset = cursorOffset + digitSpacing; // Move cursor to the right.
      // Don't allow increments > 1000000.
      if (defaultIncrement > 1000000)
      {
        defaultIncrement = 1000000;
      }
      if (cursorOffset > digitSpacing * 6)
      { // Don't overshoot or...
        cursorOffset = cursorOffset - digitSpacing;
      }
      tft.setCursor(cursorHome + cursorOffset, halfScreen + 5); //
      tft.print("_");
      digitEncoderMovement = 0;
    }
    else
    {
      // Handle movement of the cursor to the left.
      if (digitEncoderMovement == -1)
      {
        tft.fillRect(0, halfScreen + 6, data.PIXELWIDTH * .91, 20, ILI9341_BLACK); // Erase existing cursor.
        defaultIncrement = defaultIncrement * 10;
        if (defaultIncrement > 1000000)
        { // Don't go too far left
          defaultIncrement = 1000000;
        }
        else
          cursorOffset = cursorOffset - digitSpacing; // Move cursor to the left.
        if (cursorOffset < -digitSpacing * 6)         // Don't undershoot either
          cursorOffset = cursorOffset + digitSpacing;

        tft.setCursor(cursorHome + cursorOffset, halfScreen + 5);
        tft.print("_");
        digitEncoderMovement = 0;
      }
    }
    tft.setTextColor(ILI9341_GREEN);
    digitEncoderMovement = 0;
    menuEncoderMovement = 0;
    // Change digit value using the Frequency encoder.  This only has to refresh the number.
    if (frequencyEncoderMovement)
    {
      number += (int32_t)(frequencyEncoderMovement * defaultIncrement);
      tft.fillRect(insetMargin, halfScreen - 35, data.PIXELWIDTH * .91, 40, ILI9341_BLACK); // Erase the old number?

      numberSize = std::to_string(number).size();
      offset = 10 - numberSize;

      tft.setCursor(10 + digitSpacing * offset, halfScreen);
      tft.setTextSize(1);
      tft.setFont(&FreeMono24pt7b);
      tft.print(number);
      frequencyEncoderMovement = 0; // Reset encoder flag
    }
  } // end while loop

  tft.setTextSize(2); // Back to normal
  tft.setTextColor(ILI9341_WHITE);
  digitEncoderMovement = 0;
  menuEncoderMovement = 0;
  return number;
}

/*****
  Purpose: This function sets power on or off to all circuits, including the DDS.

  Parameter list:
    bool setpower

  Return value:
    void

  CAUTION:

*****/
void DisplayUtility::PowerStepDdsCirRelay(bool stepperPower, uint32_t frequency, bool circuitPower, bool relayPower)
{
  // Power up versus power down sequence needs to be different so that receiver doesn't emit noise burst.
  // Going into AutoTune, turn on the stepper last.
  if (stepperPower & circuitPower)
  {
    gpio_put(data.OPAMPPOWER, circuitPower);
    gpio_put(data.RFAMPPOWER, circuitPower);
    gpio_put(data.RFRELAYPOWER, relayPower);
    // gpio_put(data.STEPPERSLEEPNOT, stepperPower); //  Deactivating the stepper driver is important to reduce RFI.
    if (stepperPower)
      uart_write_blocking(uart1, tmcstepper.tmcDriverPower(true), 8);
    else
      uart_write_blocking(uart1, tmcstepper.tmcDriverPower(false), 8);
  }
  // Coming out of AutoTune, turn off the stepper first.  Circuit power up to measure SWR after stepper is off.
  if ((not stepperPower) & circuitPower)
  {
    // gpio_put(data.STEPPERSLEEPNOT, stepperPower); //  Deactivating the stepper driver is important to reduce RFI.
    //if (stepperPower)
    //  uart_write_blocking(uart1, tmcstepper.tmcDriverPower(true), 8);
    //else
    // Turn off stepper driver.
      uart_write_blocking(uart1, tmcstepper.tmcDriverPower(false), 8);
    dds.SendFrequency(frequency);
    gpio_put(data.OPAMPPOWER, circuitPower);
    gpio_put(data.RFAMPPOWER, circuitPower);
    gpio_put(data.RFRELAYPOWER, relayPower);
  }
    // 
  if ((not stepperPower) & (not circuitPower))
  {
    // gpio_put(data.STEPPERSLEEPNOT, stepperPower); //  Deactivating the stepper driver is important to reduce RFI.
    if (stepperPower)
      uart_write_blocking(uart1, tmcstepper.tmcDriverPower(true), 8);
    else
      uart_write_blocking(uart1, tmcstepper.tmcDriverPower(false), 8);
    gpio_put(data.OPAMPPOWER, circuitPower);
    gpio_put(data.RFAMPPOWER, circuitPower);
    gpio_put(data.RFRELAYPOWER, relayPower);
  }
  // This case is for zeroing the stepper.
    if (stepperPower & not circuitPower)
  {
    // gpio_put(data.STEPPERSLEEPNOT, stepperPower); //  Deactivating the stepper driver is important to reduce RFI.
    if (stepperPower)
      uart_write_blocking(uart1, tmcstepper.tmcDriverPower(true), 8);
    else
      uart_write_blocking(uart1, tmcstepper.tmcDriverPower(false), 8);
  }

  // Power down the DDS or set frequency.
  // if (dds)
  dds.SendFrequency(frequency); // Redundant?
  //  Power down RF amplifier and SWR circuits.
  // gpio_put(data.OPAMPPOWER, circuitPower);
  // gpio_put(data.RFAMPPOWER, circuitPower);
  // gpio_put(data.RFRELAYPOWER, relayPower);
  busy_wait_ms(500); //  Wait for relay to switch and DDS to stabilize.
}

void DisplayUtility::menuEncoderPoll()
{
  uint8_t result;
  // if ((gpio == 18) || (gpio == 20))
  // {
  result = menuEncoder.process();
  if (result != 0)
  {
    switch (result)
    {
    case DIR_CW:
      menuEncoderMovement = 1;
      digitEncoderMovement = 1;
      break;
    case DIR_CCW:
      menuEncoderMovement = -1;
      digitEncoderMovement = -1;
      break;
    }
  }
}

void DisplayUtility::freqEncoderPoll()
{
  uint8_t result;

  result = frequencyEncoder.process();
  if (result != 0)
  {
    switch (result)
    {
    case DIR_CW:
      frequencyEncoderMovement++;
      frequencyEncoderMovement2 = 1;
      break;
    case DIR_CCW:
      frequencyEncoderMovement--;
      frequencyEncoderMovement2 = -1;
      break;
    }
  }

  // if (result == DIR_CW)
  //   countEncoder = countEncoder + 1;
  // if (result == DIR_CCW)
  //   countEncoder = countEncoder - 1;
}