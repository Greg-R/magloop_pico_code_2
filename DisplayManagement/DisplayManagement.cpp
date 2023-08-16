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

#include "DisplayManagement.h"

DisplayManagement::DisplayManagement(Adafruit_ILI9341 &tft, DDS &dds, SWR &swr,
                                     StepperManagement &stepper, TmcStepper &tmcstepper, EEPROMClass &eeprom, Data &data, Button &enterbutton, Button &autotunebutton, Button &exitbutton, TuneInputs &tuneInputs) : GraphPlot(tft, dds, data), DisplayUtility(tft, dds, swr, data, tmcstepper), tft(tft), dds(dds), swr(swr),
                                                                                                                                                                                                                        stepper(stepper), eeprom(eeprom), data(data), enterbutton(enterbutton), autotunebutton(autotunebutton), exitbutton(exitbutton), tuneInputs(tuneInputs)
{
  startUpFlag = false;
  calFlag = false;
}

void DisplayManagement::Splash(std::string version, std::string releaseDate)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSerif12pt7b);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
  tft.setCursor(1, 15);
  tft.print("Pi Pico Loop Antenna Controller");
  tft.setCursor(37, 45);
  tft.print("by Gregory Raven KF5N");
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setCursor(40, 73);
  tft.print("based on a project from");
  tft.setCursor(35, 98);
  tft.print("Microcontroller Projects");
  tft.setCursor(50, 123);
  tft.print("for Amateur Radio by");
  tft.setCursor(70, 149);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.println("Al Peter  AC8GY");
  tft.setCursor(50, 175);
  tft.print("Jack Purdum  W8TEE");
  tft.setCursor(88, 205);
  tft.print("Version ");
  tft.print(version.c_str());
  tft.setCursor(50, 230);
  tft.print("Release Date ");
  tft.print(releaseDate.c_str());
  tft.setTextSize(2);
}

/*****
  Purpose: To execute the FREQ menu option
  Argument list:
    void
  Return value:
    void
*****/
void DisplayManagement::frequencyMenuOption()
{
  int SWRFlag1;
  int backCount = 0;
  long aveMinPosition;
  long frequency;
  State state;           // State machine next state.
  state = State::state1; // Begin with Select Band state.
                         // A state machine follows.  Run AutoTune when AutoTune button pushed.  Exit switch leaves while loop.
  while (true)
  {
    switch (state)
    {
    case State::state0:
      return; //  Exit frequency selection and return to top menu Freq.
    case State::state1:
      whichBandOption = SelectBand(data.bands); // state1

      // If SelectBand returns 4, this means the menu was exited without selecting a band.  Move to the top level menu Freq.
      if (whichBandOption == 4)
      {
        state = State::state0;
        break;
      }
//      if (whichBandOption == this->data.workingData.currentBand)
//        frequency = data.workingData.currentFrequency;
//      else
        frequency = data.workingData.lastFreq[whichBandOption]; // Set initial frequency for each band from Preset list
      this->data.workingData.currentBand = whichBandOption;                 //  Update the current band.
      state = State::state2;                                                // Proceed to manual frequency adjustment state.
      break;
    case State::state2:
      frequency = ChangeFrequency(whichBandOption, frequency); // Alter the frequency using encoders.  Enter button returns frequency.
      // Exit frequency change if a zero is returned.  The user pushed Exit.
      if (frequency == 0)
      {
        state = State::state1;
        break;
      }
      data.workingData.currentFrequency = frequency;
      data.workingData.lastFreq[whichBandOption] = frequency;
      eeprom.put(0, data.workingData);
      eeprom.commit(); // Write to EEPROM.
      tft.fillRect(0, 100, 311, 150, ILI9341_BLACK); // ???
      minSWRAuto = AutoTuneSWR(whichBandOption, frequency); // Auto tune here
      // After AutoTune, do full update of display with SWR vs. frequency plot:
      ShowSubmenuData(minSWRAuto, dds.currentFrequency);
      GraphAxis(whichBandOption);
      PlotSWRValueNew(whichBandOption, iMax, tempCurrentPosition, tempSWR, SWRMinPosition);
      busy_wait_ms(5000);
      break; //  state is not changed; should go back to state2.
    }        // end switch
  }          // end of while loop and state machine
  return;
}

/*****
  Purpose: Manage the manual frequency and stepper functions.
  Argument list:
    void
  Return value:
    int frequency
*****/
int DisplayManagement::manualTune()
{
  bool lastautotunebutton = true;
  bool lastexitbutton = true;
  bool lastenterbutton = true;
  bool relayState = true;  //  This will be toggled by the enter button.
  while (true)
  {
    menuEncoderPoll();
    exitbutton.buttonPushed(); // Poll exitbutton.

  //   bool lastenterbutton = true;
  
  enterbutton.buttonPushed(); // Poll exitbutton.
  if(enterbutton.pushed == true  and (not lastenterbutton)) {
    relayState = not relayState;  // Toggle relay state.
    PowerStepDdsCirRelay(relayState, 0, false, false);
  }

    if (exitbutton.pushed and not lastexitbutton)
    {
      return dds.currentFrequency; // Exit manual tuning.
    }
    if (menuEncoderMovement != 0)
    { // Allow stepper to be moved maually
      ManualStepperControl();
    }
    if (frequencyEncoderMovement != 0)
    { // Allow frequency to be changed maually.
      ManualFrequencyControl(whichBandOption);
      frequencyEncoderMovement = 0; // Doesn't reset to 0???
    }
    autotunebutton.buttonPushed(); // Poll autotunebutton.
                                   // Is this data.maxswitch protection needed here???
    if (autotunebutton.pushed == true and (not lastautotunebutton) and gpio_get(data.maxswitch))
    { // Redo the Autotune at new frequency/position
      minSWRAuto = AutoTuneSWR(data.workingData.currentBand, data.workingData.currentFrequency); // Auto tune here
      SWRMinPosition = stepper.currentPosition();                                                // Get the autotuned stepper position.
      GraphAxis(whichBandOption);
      PlotSWRValueNew(whichBandOption, iMax, tempCurrentPosition, tempSWR, SWRMinPosition);
      updateMessageTop("    Freq: Adjust - AutoTune: Refine");
      updateMessageBottom("                  Exit to Return");
    }
    lastautotunebutton = autotunebutton.pushed;
    lastexitbutton = exitbutton.pushed;
    lastenterbutton = enterbutton.pushed;
  } // end while
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
int32_t DisplayManagement::ChangeFrequency(int bandIndex, int32_t frequency)
{
  int i, changeDigit, digitSpacing, halfScreen, incrementPad, insetMargin, insetPad;
  long defaultIncrement;
  insetPad = 57; // Used to align digit indicator
  incrementPad = 05;
  digitSpacing = 10;
  insetMargin = 20;
  defaultIncrement = 1000L;
  halfScreen = PIXELHEIGHT / 2 - 25;
  updateMessageTop("                 Enter Frequency");
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
  tft.print("Tune:");
  tft.setCursor(insetMargin + 90, halfScreen + 100);
  tft.print("AutoTune Button");
  tft.setCursor(insetMargin, halfScreen + 120);
  tft.print("Exit:");
  tft.setCursor(insetMargin + 90, halfScreen + 120);
  tft.print("Exit Button");
  // Print the SWR limit frequencies to the display.
  PrintSWRlimits(fpair);
 // This method is inherited from the DisplayUtility class.
 frequency = UserNumericInput(autotunebutton, exitbutton, frequency);
 return frequency;
}

/*****
  Purpose: To get a main menu choice:  Freq, Presets, or 1st Cal.
           The method is at the top of the loop function.
           It is at the very top of the user interface.
  Argument list:

  Return value:
    int          The menu selected

  Dependencies:  Adafruit_ILI9341
*****/
DisplayUtility::TopMenuState DisplayManagement::MakeMenuSelection(TopMenuState index) // Al Mod 9-8-19
{
  int currentFrequency;
  tft.setFont();
  tft.setTextSize(2);
  int i, index_int;
  bool lastPushed;
  bool autotuneLastPushed = true;
  // Cast index to integer so it can be easily manipulated.
  index_int = static_cast<int>(index);
  // Check if initial calibration has been run.  Inform user if not.
  if (data.workingData.calibrated == 0)
  {
    //  Inform user to run Initial Calibration.
    tft.setCursor(55, 75);
    tft.setFont(&FreeSerif9pt7b);
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_RED);
    tft.print("Please run Initial Calibration!");
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(20, 125);
    tft.print("Use the Menu encoder to highlight");
    tft.setCursor(20, 150);
    tft.print("Calibrate and push Enter. Using the");
    tft.setCursor(20, 175);
    tft.print("menu encoder again, highlight Initial");
    tft.setCursor(20, 200);
    tft.print("Cal, and push Enter.");
  }
  else
  {
    //  Retrieve the last used frequency and autotune if the user pushes the AutoTune button.
    currentFrequency = data.workingData.currentFrequency;
    // Display a message to the user that the AutoTune button will tune to the
    // last used frequency prior to power-off.  This is a one-time event at power-up.
    if (startUpFlag == false)
    {
      tft.setCursor(15, 135);
      tft.setFont(&FreeSerif9pt7b);
      tft.setTextSize(1);
      tft.print("Press AutoTune for last used frequency.");
    }
  }
  tft.setFont();
  tft.setTextSize(2);
  // State Machine:
  while (true)
  {
    // Poll enterbutton.
    enterbutton.buttonPushed();
    autotunebutton.buttonPushed();
    if (autotunebutton.pushed & not autotuneLastPushed & not startUpFlag)
    {
      currentFrequency = data.workingData.currentFrequency;

      if (currentFrequency != 0)
      {

        dds.SendFrequency(currentFrequency); // Set the DDSs
                                             // Retrieve the last used frequency and autotune.
        AutoTuneSWR(data.workingData.currentBand, dds.currentFrequency);
        // Set startUpFlag to true.  This is used to skip this process after one-time use.
        startUpFlag = true;
        return TopMenuState::CALIBRATEMENU; // This will go to default in the state machine, causing a refresh of the main display.
      }
    }
    autotuneLastPushed = autotunebutton.pushed;
    //   if(enterbutton.pushed & not enterbutton.lastPushed) break;  // Looking for a low to high transition here!
    if (enterbutton.pushed)
      break; // Looking for a low to high transition here!
             //   lastPushed = enterbutton.pushed;
    menuEncoderPoll();  // Polling across the top menu selections of Freq, Presets, and Calibrate.
    if (menuEncoderMovement)
    { // Must be i (CW) or -1 (CCW)
      if (menuEncoderMovement == 1)
      {
        index_int++;
        if (index_int == data.MAXMENUES)
        { // wrap to first index
          index_int = 0;
        }
      }
      if (menuEncoderMovement == -1)
      {
        index_int--;
        if (index_int < 0)
        { // wrap to first index
          index_int = data.MAXMENUES - 1;
        }
      }
      menuEncoderMovement = 0;
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      for (i = 0; i < data.MAXMENUES; i++)
      {
        tft.setCursor(i * 100, 0);
        tft.print(menuOptions[i].c_str());
      }
      tft.setTextColor(ILI9341_BLUE, ILI9341_WHITE);
      tft.setCursor(index_int * 100, 0);
      tft.print(menuOptions[index_int].c_str());
    }
  } // end State Machine

  tft.setTextColor(ILI9341_BLUE, ILI9341_WHITE);
  tft.setCursor(index_int * 100, 0);
  tft.print(menuOptions[index_int].c_str());
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  startUpFlag = true; // Set this flag on first time use and exit from this function.
  return static_cast<TopMenuState>(index_int);  // Cast back to the state enum.
}

/*****
  Purpose: To get a band menu choice
  Argument list:
    const std::string bands[3].  Example: {"40M", "30M", "20M"}
  Return value:
    int                       the menu selected

  Dependencies:  Adafruit_ILI9341 tft
*****/
int DisplayManagement::SelectBand(const std::string bands[3])
{
  updateMessageTop("       Choose using Menu Encoder");
  EraseBelowMenu(); // Redundant???
  // int currBand[] = {40, 30, 20}; // Used???
  int i, index, where = 0;
  bool enterLastPushed = true; // Must be set to true or a false exit could occur.
  bool exitLastPushed = true;
  updateMessageBottom("             Press Enter to Select");
  tft.setTextSize(1);
  tft.setFont(&FreeSerif12pt7b);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  for (int i = 0; i < 3; i++)
  {
    tft.setCursor(110, 110 + i * 30);
    tft.print(bands[i].c_str());
  }
  tft.setCursor(110, 110);
  tft.setTextColor(ILI9341_BLUE, ILI9341_WHITE);
  tft.print(bands[0].c_str());
  index = 0;

  // State Machine.  Calling this function enters this loop and stays until Enter or Exit is pressed.
  while (true)
  {
    menuEncoderPoll();
    if (menuEncoderMovement)
    {
      if (menuEncoderMovement == 1)
      {
        index++;
        if (index == 3)
        { // wrap to first index
          index = 0;
        }
      }
      if (menuEncoderMovement == -1)
      {
        index--;
        if (index < 0)
        { // wrap to last index
          index = 2;
        }
      }
      menuEncoderMovement = 0;
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      for (int i = 0; i < 3; i++)
      {
        tft.setCursor(110, 110 + i * 30);
        tft.print(bands[i].c_str());
      }
      tft.setTextColor(ILI9341_BLUE, ILI9341_WHITE);
      tft.setCursor(110, 110 + index * 30);
      tft.print(bands[index].c_str());
    }
    // Poll buttons.
    enterbutton.buttonPushed();
    exitbutton.buttonPushed();
    if (enterbutton.pushed & not enterLastPushed)
      break; // Exit the state machine if there was a false to true transition, return selected index.
    enterLastPushed = enterbutton.pushed;
    if (exitbutton.pushed & not exitLastPushed)
      return index = 4; // 4 is a signal that the menu was exited from without making a selection.
    exitLastPushed = exitbutton.pushed;
  } // end while

  return index;
}

/*****
  Purpose: To rewrite the frequency display
  Does not reset DDS to new frequency!
  Updates frequency and stepper position.
  Does not update SWR!
  Argument list:

  Return value:
    void
*****/
void DisplayManagement::UpdateFrequency(int frequency)
{
  tft.setTextSize(1);
  tft.setFont(&FreeSerif9pt7b);
  tft.fillRect(140, 25, data.PIXELWIDTH, 20, ILI9341_BLACK);
  tft.setCursor(100, 40);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.print("FREQ ");
  tft.setFont(&FreeSerif12pt7b);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.print(frequency);
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.print("  p ");
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.print(stepper.currentPosition());
}


/*****
  Purpose: Do initial Calibrate.  Does not assume band edge counts to begin

  Argument list:
    void

  Return value:
    void
*****/

void DisplayManagement::DoFirstCalibrate()
{
  int bandBeingCalculated;
  int i, j, whichLine;
  calFlag = true;
  long localPosition, minCount, frequency;
  float currentSWR;
  bool lastexitbutton = true;
  EraseBelowMenu();
  updateMessageTop("                 Sweeping Stepper");
  bandBeingCalculated = 0;
  PowerStepDdsCirRelay(true, 0, false, false); //  Power up circuits.
  updateMessageTop("                   Setting to Zero");
  stepper.ResetStepperToZero();
  updateMessageBottom("                 Initial Calibration");
  position = 0;
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setCursor(0, 65);
  tft.print("Frequency               SWR       Count"); // Table header
  tft.setCursor(0, 90);                                 // Read to show mins...
  whichLine = 0;                                  // X coord for mins
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); // Table data

  for (i = 0; i < data.MAXBANDS; i = i + 1)
  { // For the 3 bands...
    for (j = 0; j < 2; j = j + 1)
    {
      this->data.workingData.currentBand = i;             // Used by SWRdataAnalysis()
      frequency = this->data.workingData.bandEdges[i][j]; // Select a band edge to calibrate
      this->data.workingData.currentFrequency = frequency;
      PowerStepDdsCirRelay(true, frequency, true, true); //  Power up circuits, close relay.  Leave on until cal complete.
      updateMessageTop("                  Coarse Tuning");
      while (true)
      {
        position = stepper.currentPosition();
        // Coarse sweep:
        while (swr.ReadSWRValue() > 4.0)
        {                                                      // Move stepper in CW direction in larger steps for SWR > 4 (upper limit of the graph)
          position = position + data.workingData.coarse_sweep; // This value will depend on the capacitor used.
          ShowSubmenuData(swr.ReadSWRValue(), dds.currentFrequency);
          stepper.MoveStepperToPosition(position);
        }
        // AutoTune when SWR goes below 4.
        minSWRAuto = AutoTuneSWR(i, frequency); // AutoTuneSWR() returns 0 if failure.
        if (minSWRAuto == 0.0)
          return;

        if (minSWRAuto < TARGETMAXSWR)
        { // Ignore values greater than Target Max
          data.workingData.bandLimitPositionCounts[i][j] = SWRMinPosition;
          // Write the position to the upper frequency.  This is to prevent AutoTune from using the default of zero.
          if (j == 0)
            data.workingData.bandLimitPositionCounts[i][1] = SWRMinPosition;
          tft.setCursor(0, 90 + whichLine * TEXTLINESPACING);
          if (dds.currentFrequency < 10000000)
          {
            tft.print(" ");
          }
          tft.print(dds.currentFrequency);
          tft.setCursor(150, 90 + whichLine * TEXTLINESPACING);
          tft.print(minSWRAuto);
          tft.setCursor(230, 90 + whichLine * TEXTLINESPACING);
          tft.print(SWRMinPosition);
          whichLine++; // Ready for next line of output
          break;       // Leave the while loop. This sends control to next edge
        }
      }
      //  position = stepper.currentPosition() - 50;
    } // end for (j
  } // end for (i
  PowerStepDdsCirRelay(false, 0, false, false);  // Power down, calibration is complete.
  //  Set the calibrated flag in workingData.
  data.workingData.calibrated = 1;
  eeprom.put(0, data.workingData);
  eeprom.commit(); // This writes a page to Flash memory.  This includes the position counts
                   // and preset frequencies.
  // Slopes can't be computed until the actual values are loaded from flash:
  data.computeSlopes();
  calFlag = false;
  //PowerStepDdsCirRelay(false, 0, false, false);  // !!!
  updateMessageBottom("        Initial Calibration Complete");
  updateMessageTop("                    Press Exit");
  while (true)
  {
    exitbutton.buttonPushed(); // Poll exitbutton.
    if (exitbutton.pushed and not lastexitbutton)
      break; // Check for positive edge.
    lastexitbutton = exitbutton.pushed;
  }
}

/*****
  Purpose: To set the band end point counts for a single band for intermediate calibration
  Does not reset the stepper.

  Argument list:
    void

  Return value:
    void
*****/
void DisplayManagement::DoSingleBandCalibrate(int whichBandOption)
{ // Al Added 4-18-20
  int bandBeingCalculated = 0;
  int i, j, whichLine;
  long localPosition, minCount, frequency;
  float currentSWR;
  calFlag = true;
  PowerStepDdsCirRelay(true, 0, true, true); // Power up for duration of cal process.
  tft.fillRect(0, 46, 340, 231, ILI9341_BLACK);
  tft.drawFastHLine(0, 45, 320, ILI9341_RED);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setFont(&FreeSerif9pt7b);
  tft.setCursor(0, 65);
  tft.print("Frequency               SWR       Count"); // Table header
  tft.setCursor(0, 90);                                 // Read to show mins...

  whichLine = 0;                                  // X coord for mins
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); // Table data
  updateMessageBottom("            Single Band Calibrate");
  for (j = 0; j < 2; j++)
  {                                                             // For each band edge...
    frequency = data.workingData.bandEdges[whichBandOption][j]; // Select a band edge
    while (true)
    {
      if (gpio_get(data.maxswitch) != HIGH)
      {                               // At the end stop switch?
        stepper.ResetStepperToZero(); // Yep, so leave.
        return;
      }
      currentSWR = swr.ReadSWRValue();
      updateMessageTop("Auto Tuning");
      minSWRAuto = AutoTuneSWR(whichBandOption, frequency);
      ShowSubmenuData(minSWRAuto, dds.currentFrequency); // Update SWR value
      if (minSWRAuto < TARGETMAXSWR)
      { // Ignore values greater than Target Max
        data.workingData.bandLimitPositionCounts[whichBandOption][j] = SWRMinPosition;
        tft.setCursor(0, 90 + whichLine * TEXTLINESPACING);
        if (dds.currentFrequency < 10000000)
        {
          tft.print(" ");
        }
        tft.print(dds.currentFrequency);
        tft.setCursor(150, 90 + whichLine * TEXTLINESPACING);
        tft.print(minSWRAuto);
        tft.setCursor(230, 90 + whichLine * TEXTLINESPACING);
        tft.print(SWRMinPosition);
        whichLine++; // Ready for next line of output
        break;       // This sends control to next edge
      }
    }
    position = data.workingData.bandLimitPositionCounts[whichBandOption][1] - 50;
  } // end for (j
  position = SWRFinalPosition + 50;
  eeprom.commit(); // Write values to EEPROM
  updateMessageTop("                     Press Exit");
  updateMessageBottom("     Single Band Calibrate Complete");
  //PowerStepDdsCirRelay(false, 0, false, false); // !!!
  calFlag = false;
  while (exitbutton.pushed == false)
  {
    exitbutton.buttonPushed(); // Poll exitbutton.
    if (exitbutton.pushed)
      break;
  }
}

/*****
  Purpose: State machine to select a preset frequency and then AutoTune to that frequency.

  Parameter list:
    int whichBandOption
    int submenuIndex          which of presets

  Return value:
    void

  CAUTION:

*****/
void DisplayManagement::ProcessPresets()
{
  int i;
  int backCount = 0;
  long frequency;
  State state = State::state0;
  state = State::state1; // Begin with Select Band state.
  while (true)
  {
    switch (state)
    {
    case State::state0:
      return; // Return to top level.
    case State::state1:
      whichBandOption = SelectBand(data.bands); // Select the band to be used
      this->data.workingData.currentBand = whichBandOption;
      // If SelectBand returns 4, the user exited before selecting a band.  Return to top menu.
      if (whichBandOption == 4)
      {
        state = State::state0;
        break;
      }
      submenuIndex = 0;
      state = State::state2;
      break;
    case State::state2:
      frequency = SelectPreset(); // This method contains code to modify and save the presets.
      state = State::state3;      // Return to this point and you MUST go to state3 (autotune)!
      if (frequency == 0)
        state = State::state1; // User pushed exit, return to band select.
      break;
    case State::state3: // Run AutoTuneSWR() at the selected preset frequency.
      this->data.workingData.currentFrequency = frequency;
      eeprom.put(0, data.workingData);
      eeprom.commit();
      minSWRAuto = AutoTuneSWR(whichBandOption, data.workingData.currentFrequency);
      ShowSubmenuData(minSWRAuto, dds.currentFrequency);
      GraphAxis(whichBandOption);
      PlotSWRValueNew(whichBandOption, iMax, tempCurrentPosition, tempSWR, SWRMinPosition);
      busy_wait_ms(5000);
      state = State::state2; // Move to Select Preset state.
      break;
      default:  // Should never go here!
      return; // Return nothing.  Should break things.
    }
  }
}

// Used in ProcessPreset to select a particular preset frequency.
// If the Enter button is pressed, the preset frequency can be changed and saved.
int DisplayManagement::SelectPreset()
{
  int frequency;
  bool lastexitbutton = true;
  bool lastenterbutton = true;
  bool lastautotunebutton = true;
  menuEncoderState = 0;
  state = State::state0; // Enter state0 which does graphics.

  //  Preset state selection machine
  while (true)
  {
    // Poll 3 buttons:
    autotunebutton.buttonPushed();
    enterbutton.buttonPushed();
    exitbutton.buttonPushed();

    switch (state)
    {
    case State::state0: // This state does the graphics.
      updateMessageTop("Menu Encoder to select, push AutoTune");
      EraseBelowMenu();
      tft.setTextSize(1);
      tft.setFont(&FreeSerif12pt7b);
      tft.setCursor(175, 125);
      tft.print("Press Enter to");
      tft.setCursor(175, 155);
      tft.print("adjust preset");
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); // Show presets for selected band
      for (int i = 0; i < data.PRESETSPERBAND; i++)
      {
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        tft.setCursor(30, 70 + i * 30);
        tft.print(i + 1);
        tft.print(".");
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        tft.setCursor(65, 70 + i * 30);
        tft.print(data.workingData.presetFrequencies[whichBandOption][i]);
      }
      tft.setTextColor(ILI9341_MAGENTA, ILI9341_WHITE);
      tft.setCursor(65, 70 + submenuIndex * 30);
      tft.print(data.workingData.presetFrequencies[whichBandOption][submenuIndex]);
      state = State::state1;
      break;

    case State::state1: // This state reads the encoders and button pushes.
      menuEncoderPoll();
      if (menuEncoderMovement == 1)
      { // Turning clockwise
        RestorePreviousPresetChoice(submenuIndex, whichBandOption);
        submenuIndex++;
        if (submenuIndex > data.PRESETSPERBAND - 1)
          submenuIndex = 0;
        HighlightNewPresetChoice(submenuIndex, whichBandOption);
        menuEncoderMovement = 0;
      }
      if (menuEncoderMovement == -1)
      { // Tuning counter-clockwise
        RestorePreviousPresetChoice(submenuIndex, whichBandOption);
        submenuIndex--;
        if (submenuIndex < 0)
          submenuIndex = data.PRESETSPERBAND - 1;
        HighlightNewPresetChoice(submenuIndex, whichBandOption);
        menuEncoderMovement = 0;
      }
      if (exitbutton.pushed & not lastexitbutton)
        return frequency = 0; // Exit Preset Select if requested by user.
      lastexitbutton = exitbutton.pushed;
      if (enterbutton.pushed & not lastenterbutton)
      {
        frequency = data.workingData.presetFrequencies[whichBandOption][submenuIndex];
     frequency = tuneInputs.ChangeParameter(frequency);
        // Save the preset to the EEPROM.
        data.workingData.presetFrequencies[whichBandOption][submenuIndex] = frequency;
        eeprom.put(0, data.workingData);
        eeprom.commit();
        //  Need to refresh graphics, because they were changed by ChangeFrequency!
        state = State::state0; // Refresh the graphics.
        //  lastenterbutton = enterbutton.pushed;
      }
      lastenterbutton = enterbutton.pushed;
      lastexitbutton = exitbutton.pushed;
      break;
    default:
      break;
    } // end switch of state machine
      // Process button pushes after switch statement, but before end of while block.
    if (autotunebutton.pushed & not lastautotunebutton)
      break; // Exit preset select, return the frequency and proceed to AutoTune.
    lastautotunebutton = autotunebutton.pushed;

  }                                                                              // end while Preset state selection machine
  frequency = data.workingData.presetFrequencies[whichBandOption][submenuIndex]; //  Retrieve the selected frequency.
  return frequency;
}


// This is the primary auto-tuning state-machine which minimizes SWR.
// The stepper should be positioned below the minimum SWR frequency.
// This is done either by starting at stepper position 0, or using a calculated estimation.
float DisplayManagement::AutoTuneSWR(uint32_t band, uint32_t frequency)
{
  minSWR = 100.0;
  minSWRAuto = 3.0;
  int i = 0;
  updateMessageTop("                  Coarse Tuning");
  if (calFlag == true)
    position = stepper.currentPosition();
  // Backup data.backlash counts to approach from CW direction
  // This is an estimation of the position based on results from the initial calibration band ends positions.
  else
  {
    position = -data.workingData.backlash + data.workingData.bandLimitPositionCounts[band][0] + static_cast<int>(static_cast<float>(frequency - data.workingData.bandEdges[band][0]) / data.hertzPerStepperUnitVVC[band]);
    // Power to the stepper, bridge, and relay, unless calibrating.  If calibrating, calibration routine will control power.
    PowerStepDdsCirRelay(true, frequency, true, true);
    //  Move the stepper to the approximate location based on the current frequency:
    stepper.MoveStepperToPosition(position);
  }
  SWRMinPosition = 100000;
  tempSWR.clear(); // Clear vectors.
  tempCurrentPosition.clear();
  updateMessageTop("                    Auto Tuning");
  // The coarse autotune loop:
  while (minSWR > 2.9) // Need slight overlap with fine tune.
  {
    position = position + data.workingData.coarse_sweep; // Increment forward by
    stepper.MoveStepperToPosition(position);
    minSWR = swr.ReadSWRValue();                   // Save SWR value
    ShowSubmenuData(minSWR, dds.currentFrequency); // Update display during sweep.
  }
  // The fine autotune loop:
  while ((minSWR < 3) & (stepper.currentPosition() < (SWRMinPosition + data.workingData.backlash)))
  {
    // Test to find minimum SWR value
    minSWR = swr.ReadSWRValue();                   // Save SWR value
    ShowSubmenuData(minSWR, dds.currentFrequency); // Update display during sweep.
    if (minSWR < minSWRAuto)
    {
      minSWRAuto = minSWR;                        // If this measurement is lower, save it.
      SWRMinPosition = stepper.currentPosition(); // Save the stepper position.
      SWRMinIndex = i;                            // Save the array index of the minimum.
    }
    tempSWR.push_back(minSWR);
    tempCurrentPosition.push_back(stepper.currentPosition());
    i = i + 1;
    position = position + 1; // Increment forward by 1 step.
    stepper.MoveStepperToPosition(position);
  } // end while

  // Move to optimal position and report results.
  stepper.MoveStepperToPosition(SWRMinPosition - data.workingData.backlash); // Back up position to take out backlash.
  stepper.MoveStepperToPosition(SWRMinPosition);                             // Move to final position in CW direction.
  
  // Power down all circuits except in calibrate mode.
  if (calFlag == false) {
    //  Power down the stepper before measuring VSWR!  Delay to allow TMC to slowly decrease motor hold current.
    busy_wait_ms(1000);
    PowerStepDdsCirRelay(false, frequency, true, true);  // Disengage stepper driver.  Leave circuits on to measure SWR.
    minSWR = swr.ReadSWRValue();                         // Measure VSWR in the final position.
  //  Now shut the rest of the circuits down.
    PowerStepDdsCirRelay(false, 0, false, false);
    }
  else minSWR = swr.ReadSWRValue(); 
  iMax = i;                                      // Max value in array for plot.
  ShowSubmenuData(minSWR, dds.currentFrequency); // Update SWR value.
  updateMessageTop("               AutoTune Success");
  // Compute the upper and lower frequencies at which SWR ~2:1.
  SWRdataAnalysis();

  return minSWR;
}

// Manual control functions were moved from SWR.
/*****
  Purpose: Manual Setting the Frequency

  Parameter list:

  Return value:
    void
*****/
void DisplayManagement::ManualFrequencyControl(int whichBandOption)
{
  updateMessageTop("     Press Enter: Move to Freq");
  int i, k, yIncrement, xIncrement;
  int stepIncr;
  int frequency = dds.currentFrequency;
  int frequencyOld = frequency;
  long tempTime; // Used???
  xIncrement = (XAXISEND - XAXISSTART) / 3;
  yIncrement = (YAXISEND - YAXISSTART) / 3;
  int xDotIncrement = 10;
  int yTick = YAXISSTART + 5;
  bool lastenterbutton = true;
  frequencyEncoderMovement = 0;
  GraphAxis(whichBandOption);
  if (frequencyEncoderMovement2 != 0)
  {
    frequencyOld = dds.currentFrequency;
    // Enter this loop until enterbutton is pushed.
    while (true)
    {
      enterbutton.buttonPushed(); // Poll enterbutton.
      if (enterbutton.pushed and not lastenterbutton)
        break;
      lastenterbutton = enterbutton.pushed; // Used to make sure enterbutton uses edge.
      if (frequencyEncoderMovement2 != 0)
      {
        frequency = dds.currentFrequency + frequencyEncoderMovement2 * 1000;
        dds.SendFrequency(frequency);
        ShowSubmenuData(swr.ReadSWRValue(), frequency); // Updates display only.
        frequencyEncoderMovement2 = 0;
      }
    }
    updateMessageTop("                  Exit to Return");
    updateMessageBottom("     Freq: Adjust - AutoTune: Refine");
    dds.SendFrequency(frequency); // Redundant???
    position = stepper.currentPosition() + ((frequency - frequencyOld) / (data.hertzPerStepperUnitVVC[whichBandOption]));
    stepper.MoveStepperToPosition(position); // Al 4-20-20
    int k = 0;
    frequencyEncoderMovement = 0;
    frequencyEncoderMovement2 = 0;
  }
  PlotNewStartingFrequency(whichBandOption);
  ShowSubmenuData(swr.ReadSWRValue(), frequency);
}

/*****
  Purpose: Manual Setting the Stepper

  Parameter list:

  Return value:
    void
*****/
void DisplayManagement::ManualStepperControl()
{
  long position;
  position = stepper.currentPosition() + menuEncoderMovement;
  stepper.MoveStepperToPosition(position);
  ShowSubmenuData(swr.ReadSWRValue(), dds.currentFrequency);
  menuEncoderMovement = 0;
}


/*****
  Purpose: Select and execute user selected Calibration algorithm.
           Manage the display, Enter and Exit buttons

  Parameter list:


  Return value:
    void

  CAUTION:

*****/
void DisplayManagement::CalibrationMachine()
{
  int i;
  bool lastexitbutton = true;
  std::string cals[] = {"Initial Cal", "Zero Stepper", "Hardware Settings"};
  EraseBelowMenu();
  state = State::state0; // Enter state0.
  // menuIndex = mode::PRESETSMENU;         // Superfluous???
  while (true)
  {
    switch (state)
    {
    case State::state0:         // Select Calibration algorithm.
      i = SelectBand(cals) + 1; // Calibration states are 1,2,3.
      if (i == 5)
        return;         // No selection in Calibrate menu; exit machine and return to top level.
      state = (State)i; // Cast i to State enum type.
      break;
    case State::state1: // Initial Calibration
      DoFirstCalibrate();
      state = State::state0;
      lastexitbutton = true; // Must set true here, or will jump to top level.
      break;
    case State::state2: // Zero Stepper
      PowerStepDdsCirRelay(true, 0, false, false);
      stepper.ResetStepperToZero();
      //PowerStepDdsCirRelay(false, 0, false, false); // !!!
      state = State::state0; // Return to Calibration select after exiting state2.
      lastexitbutton = true; // Must set true here, or will jump to top level.
      break;
    case State::state3: // Hardware parameters
      tuneInputs.SelectParameter();
      state = State::state0;
      lastexitbutton = true; // Must set true here, or will jump to top level.
      break;
    }                          // end switch
    exitbutton.buttonPushed(); // Poll exitbutton.
    if (exitbutton.pushed and not lastexitbutton)
      break;                            // Break from while if exit button is pushed.
    lastexitbutton = exitbutton.pushed; // Make sure exit happens on edge.
  }                                     // end while
}


/*****
  Purpose: This function analyzes the SWR data and determines the frequency
           limits at which SWR is approximately 2:1.

  Parameter list:
    float SWRarray[500], uint32_t position[500] (array of stepper positions)
SWRdataAnalysis
  Return value:
    std::pair<uint32_t, uint32_t>

  CAUTION:

*****/
void DisplayManagement::SWRdataAnalysis()
{
  // long flow, fhigh;
  int32_t fcenter;
  int32_t flow = 0;
  int32_t fhigh = 0;
  int posLowIndex = 0;
  int posHighIndex = 0;
  for (int i = 0; i < 500; i = i + 1)
  {
    if ((tempSWR[i] < 2.0) and (tempSWR[i] > 0.9))
    {
      posLowIndex = i; // This is a stepper position!
      break;           // Exit, and proceed to process upper half.
    }
    else
      posLowIndex = 0; // For case where all values < 2.0.
  }
  for (int i = posLowIndex + 1; i < 500; i = i + 1)
  {
    if ((tempSWR[i] > 2.0) and (tempSWR[i] > 0.9))
    {
      posHighIndex = i; // This is a stepper position!
      break;            // Exit, found upper 2:1 position.
    }
    else
      posHighIndex = 499; // For case where all values < 2.0.
  }
  // Now calculate the end frequencies over which SWR is <2.0:1.
  // SWRMinPosition is the index the desired center frequency for AutoTune.
  // Need the band index to retrieve the slope.
  fcenter = dds.currentFrequency;
  flow = fcenter - (SWRMinPosition - tempCurrentPosition[posLowIndex]) * static_cast<int32_t>(this->data.hertzPerStepperUnitVVC[this->data.workingData.currentBand]);
  fhigh = fcenter + (tempCurrentPosition[posHighIndex] - SWRMinPosition) * static_cast<int32_t>(this->data.hertzPerStepperUnitVVC[this->data.workingData.currentBand]);
  fpair = {flow, fhigh};
}

/*****
  Purpose: This function analyzes the SWR data and determines the frequency
           limits at which SWR is approximately 2:1.

  Parameter list:
    float SWRarray[500], long position[500]

  Return value:
    void

  CAUTION:

*****/
void DisplayManagement::PrintSWRlimits(std::pair<uint32_t, uint32_t> fpair)
{
  tft.setFont(&FreeSerif9pt7b);
  tft.setCursor(10, 135);
  tft.print("flow = "); 
  tft.setCursor(64, 135);
  tft.print(fpair.first);
  tft.setCursor(160, 135);
  tft.print("fhigh = "); 
  tft.setCursor(218, 135);
  tft.print(fpair.second);
}