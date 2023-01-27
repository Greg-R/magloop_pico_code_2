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

#include "Button.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

Button::Button(uint gpio): gpio(gpio) {
  pushed = false;
  state = State::state0;
}

// Default constructor.
//Button::Button() {}

/*****
  Purpose: Initialize a GPIO to be used as a pushbutton input.
  A pull-up will be used.  The input is true when the button is not pushed.
  The input becomes false when the button is pushed.
  Input hysteresis is active by default.
  Argument list:
    uint gpio
  Return value:
    void
*****/
void Button::initialize() {
gpio_set_function(gpio, GPIO_FUNC_SIO);
gpio_set_dir(gpio, GPIO_IN);
gpio_pull_up(gpio);
}

/*****
  Purpose: De-bouncing state-machine.  This function must be run in a fast loop.
  This function sets pushed to true if the button is pushed.
  Argument list:
    void
  Return value:
    void
*****/
void Button::buttonPushed() {
//uint64_t time;
switch(state) {
  case State::state0:
    if(gpio_get(gpio) == false) {  // Button pushed, proceed to state1.
      state = State::state1;
      startTime = time_us_64();
    }
    else {                         // Not pushed, stay in state0.
      state = State::state0;
    }
    break;
  case State::state1:                          // This state may need timing added.
  time = time_us_64();
  if((time - startTime) > debounceInterval) {
      if(gpio_get(gpio) == false) {
        pushed = true;  // Button is de-bounced and push detected.
        state = State::state2;
     //   break;
      }
      else {
        state = State::state0;  // Go back to state0.
      //  break;
      }
      break;
    }
    state = State::state1;  // Stay in state1 until debounceInterval has elapsed.
    break;
  case State::state2:
    if(gpio_get(gpio) == false) {  // Button still pushed; stay here until it is not.
      state = State::state2;
    }
    else {
      state = State::state3;   // Button may be released; proceed to state3.
    }
    break;
  case State::state3:
    if(gpio_get(gpio) == true) {  // Button is released.  Go back to state0.
      state = State::state0;
      pushed = false;             // Button is released.
    }
    else {
      state = State::state2;     // Button is still pushed.  Back to state2.
    }
    break;
    default:
    break;
  }  // end switch
}  // end buttonPushed()
