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

#include "Hardware.h"

Hardware::Hardware(Adafruit_ILI9341 &tft, DDS &dds, SWR &swr, Button &enterbutton, Button &autotunebutton, Button &exitbutton, Data &data,
                   StepperManagement &stepper, TmcStepper &tmcstepper):DisplayUtility(tft, dds, swr, data, tmcstepper), tft(tft), dds(dds), swr(swr), enterbutton(enterbutton),
                   autotunebutton(autotunebutton), exitbutton(exitbutton), data(data), stepper(stepper), tmcstepper(tmcstepper)
{
   //parameterNames = {"Zero Offset", "Backlash", "Coarse Sweep", "Acceleration", "Speed"};
   maxclose = false;
   zeroclose = false;
   submenuIndex = 0;
   pauseTime = 15;
};

void Hardware::SWR_Test() {
  tft.fillScreen(ILI9341_BLACK); // Clear display.
  tft.setTextColor(ILI9341_GREEN);
  //  Start the DDS:
  PowerStepDdsCirRelay(false,  7000000, true, false); // Activate SWR circuits.
  float swrValue;
  // Set up a 1 minute timer.  The test ends at 1 minute.
  absolute_time_t timestamp1minute;
  uint64_t oneMinute = pauseTime * 1000000;
  timestamp1minute._private_us_since_boot = time_us_64() + oneMinute;
  tft.setTextSize(1);
  // Main loop state machine:
  while (not time_reached(timestamp1minute))  // 1 minute timer
  {
   tft.setCursor(10, 20);
   tft.print("SWR Bridge Test");
   tft.setCursor(10, 70);
   tft.print("SWR = ");
   tft.setCursor(85, 70);
   tft.print(swr.ReadSWRValue());
   // Read and print forward ADC integer.
   adc_select_input(1);
   tft.setCursor(10, 100);
   tft.print("Forward ADC");
   tft.setCursor(155, 100);
   tft.print(adc_read());
  // Read and print reverse ADC integer.
   adc_select_input(0);
   tft.setCursor(10, 130);
   tft.print("Reverse ADC");
   tft.setCursor(155, 130);
   tft.print(adc_read());
   busy_wait_ms(1000);
   // Blank out old readings
   tft.fillRect(81, 53, 70, 20, ILI9341_BLACK);
   tft.fillRect(150, 83, 70, 20, ILI9341_BLACK);
   tft.fillRect(150, 113, 70, 20, ILI9341_BLACK);
  }   // while(1)  (end of main loop)
  PowerStepDdsCirRelay(false,  0, false, false);  // Deactivate SWR circuits.
   tft.fillScreen(ILI9341_BLACK); // Clean up display.
}

// Button Test
void Hardware::ButtonTest() {
  std::vector<std::string> buttons = {"Enter Button", "AutoTune Button", "Exit Button", "Zero Switch", "Max Switch"};
  tft.fillScreen(ILI9341_BLACK); // Clear display.
  tft.setTextSize(1);
  tft.setCursor(10, 15);
  tft.print("Input Test");
/*    tft.setCursor(10, 40);
    tft.print("Enter Button");
    tft.setCursor(10, 130);
    tft.setCursor(10, 70);
    tft.print("AutoTune Button");
    tft.setCursor(10, 100);
    tft.print("Exit Button");
    tft.setCursor(10, 190);
    tft.print("Zero Switch");
    tft.setCursor(10, 220);
    tft.print("Max Switch");
    */
   // Draw the buttons/switches list.
          for (int i = 0; i < buttons.size(); i++)
      {
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        tft.setFont(&FreeSerif12pt7b);
        tft.setCursor(30, 70 + i * 30);  // 30 pixels vertical spacing.
        tft.print(buttons[i].c_str());
      }
  // Set up a 1 minute timer.  The test ends at 1 minute.
  absolute_time_t timestamp1minute;
  uint64_t oneMinute = pauseTime * 1000000;
  timestamp1minute._private_us_since_boot = time_us_64() + oneMinute;
    while (not time_reached(timestamp1minute))
  //while(true)
  { 
    // Pushbuttons Test  
      enterbutton.buttonPushed();
    if(enterbutton.pushed) {
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(200, 70);
      tft.print("PUSHED");
    } else {
      //tft.fillRect(195, 25, 100, 20, ILI9341_WHITE); 
      tft.setCursor(200, 70);
      tft.setTextColor(ILI9341_BLACK);
      tft.print("PUSHED");
    }    
    autotunebutton.buttonPushed();
    if(autotunebutton.pushed) {
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(200, 100);
      tft.print("PUSHED");
    } else {
      //tft.fillRect(195, 53, 100, 20, ILI9341_BLACK);
      tft.setCursor(200, 100);
      tft.setTextColor(ILI9341_BLACK);
      tft.print("PUSHED");
    }
       exitbutton.buttonPushed();
    if(exitbutton.pushed) {
            tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(200, 130);
      tft.print("PUSHED");
    } else {
      tft.setCursor(200, 130);
      tft.setTextColor(ILI9341_BLACK);
      tft.print("PUSHED");
    }
    //     Antenna switch tests
    if(gpio_get(data.zeroswitch) == 0) {
            tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(200, 160);
      tft.print("CLOSED");
    } else {
          tft.setCursor(200, 160);
          tft.setTextColor(ILI9341_BLACK);
      tft.print("CLOSED");
    }   

    if(gpio_get(data.maxswitch) == 0) {
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(200, 190);
      tft.print("CLOSED");
    } else {
            tft.setCursor(200, 190);
            tft.setTextColor(ILI9341_BLACK);
      tft.print("CLOSED");
    }   
  
  }   // while(1)  (end of main loop)
     tft.fillScreen(ILI9341_BLACK); // Clean up display.
}


// Encoder Test
void Hardware::EncoderTest() {
  std::vector<std::string> encoders = {"FREQ Encoder", "MENU Encoder"};
  tft.fillScreen(ILI9341_BLACK); // Clear display.
  tft.setTextSize(1);
  tft.setCursor(10, 15);
  tft.print("Encoder Test");
  tft.setCursor(10, 40);
    tft.setCursor(10, 130);
    tft.print("FREQ Encoder");
    tft.setCursor(10, 160);
    tft.print("MENU Encoder");
  int freqEncoderCount = 0;
  int menuEncoderCount = 0;
  // Set up a 1 minute timer.  The test ends at 1 minute.
  absolute_time_t timestamp1minute;
  uint64_t oneMinute = pauseTime * 1000000;
  timestamp1minute._private_us_since_boot = time_us_64() + oneMinute;
    while (not time_reached(timestamp1minute))
  //while(true)
  { 
    // Encoders Test
    freqEncoderPoll();
    menuEncoderPoll();
    if(frequencyEncoderMovement2) {
      freqEncoderCount += frequencyEncoderMovement2;
      frequencyEncoderMovement2 = 0;
      tft.fillRect(192, 112, 50, 20, ILI9341_BLACK);
    tft.setCursor(200, 130);
    tft.setTextColor(ILI9341_WHITE);
    tft.print(freqEncoderCount);
    }   

    if(menuEncoderMovement) {
      menuEncoderCount += menuEncoderMovement;
      menuEncoderMovement = 0;
      tft.fillRect(192, 142, 50, 20, ILI9341_BLACK);
       tft.setCursor(200, 160);
       tft.setTextColor(ILI9341_WHITE);
   tft.print(menuEncoderCount);
    }
  }   // while(1)  (end of main loop)
     tft.fillScreen(ILI9341_BLACK); // Clean up display.
}

  // Motor Test
void Hardware::MotorTest() {
  tft.fillScreen(ILI9341_BLACK); // Clear display.
  PowerStepDdsCirRelay(true,  0, false, false); // Activate motor circuit.
     tft.setCursor(10, 20);
   tft.print("Stepper Motor Test");
  // Set up a 1 minute timer.  The test ends at 1 minute.
  absolute_time_t timestamp1minute;
  uint64_t oneMinute = pauseTime * 1000000;
  timestamp1minute._private_us_since_boot = time_us_64() + oneMinute;
    while (not time_reached(timestamp1minute))
  {
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(10, 90);
  tft.print("Move motor towards max");
  stepper.moveTo(500);
  stepper.runToPosition();
  busy_wait_ms(1000);
  tft.fillRect(10, 74, 300, 20, ILI9341_BLACK);
  tft.setCursor(10, 90);
  tft.print("Return towards zero");
  stepper.moveTo(0);
  stepper.runToPosition();
  busy_wait_ms(1000);
  tft.fillRect(10, 74, 300, 20, ILI9341_BLACK);
  }   // while(1)  (end of main loop)
    PowerStepDdsCirRelay(false,  0, false, false); // Deactivate motor circuit.
     tft.fillScreen(ILI9341_BLACK); // Clean up display.
  }

  void Hardware::SelectTest()
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
      // Show current hardware tests
      for (int i = 0; i < tests.size(); i++)
      {
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        tft.setFont(&FreeSerif12pt7b);
        tft.setCursor(30, 70 + i * 30);
        tft.print(i + 1);
        tft.print(".");
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        tft.setCursor(65, 70 + i * 30);
        tft.print(tests[i].c_str());
   //     tft.setCursor(215, 70 + i * 30);
   //     tft.setFont(&FreeMono12pt7b);
    //    tft.print(tests[i].c_str());
      }
      
      tft.setTextColor(ILI9341_MAGENTA, ILI9341_WHITE);
    //  tft.setCursor(30, 70 + submenuIndex * 30);
    //  tft.print(1);
    //  tft.print(".");
      tft.setCursor(65, 70 + submenuIndex * 30);
      tft.print(tests[submenuIndex].c_str());  // Highlight selection.
      state = State::state1;
      break;

    case State::state1: // This state reads the encoders and button pushes.
    menuEncoderPoll();
      if (menuEncoderMovement == 1)
      { // Turning clockwise
        RestorePreviousChoice(submenuIndex);
        submenuIndex++;
        if (submenuIndex > (tests.size() - 1))
          submenuIndex = 0;
        HighlightNextChoice(submenuIndex);
        menuEncoderMovement = 0;
      }
      if (menuEncoderMovement == -1)
      { // Tuning counter-clockwise
        RestorePreviousChoice(submenuIndex);
        submenuIndex--;
        if (submenuIndex < 0)
          submenuIndex = (tests.size() - 1);
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
            ButtonTest();
            break;
           case 1:
            EncoderTest();
            break;
          case 2:
            SWR_Test();
            break;
          case 3:
            MotorTest();
            break;
          default:
          break;
        } 
        lastexitbutton = true;  // Prevents exit button from skipping a level.
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

// These functions are slightly different than the ones in the DisplayUtility class.
void Hardware::RestorePreviousChoice(int submenuIndex)
{
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); // Restore old background
  tft.setCursor(65, 70 + submenuIndex * 30);
  tft.print(tests[submenuIndex].c_str());
}

void Hardware::HighlightNextChoice(int submenuIndex)
{
  tft.setTextColor(ILI9341_MAGENTA, ILI9341_WHITE); // Highlight new preset choice
  tft.setCursor(65, 70 + submenuIndex * 30);
  tft.print(tests[submenuIndex].c_str());
}

