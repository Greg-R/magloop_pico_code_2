
#pragma once
#include <stdint.h>
#include <array>
#include "hardware/uart.h"

class TmcStepper {

public:

  // Reverse the stepper direction via UART.  This register also activates register for step size!
  // Set this register once at start-up.
  // GCONF register 0x0                  Sync        Slave Addr  RW+Reg Addr 32 bits Data  31 -> 0                           CRC
  std::array<uint8_t, 8> reverse =      {0b00000101, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b11001001, 0b00000000};
  std::array<uint8_t, 8> forward =      {0b00000101, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b11000001, 0b00000000};
  // Now adjust the step size.  Register 0x6C.  Default 0x10000053           32 bits Data                      UART^^MSTEP^ISCALE
  // All have the driver configured off.
  std::array<uint8_t, 8> stepsize256 =  {0b00000101, 0b00000000, 0b11101100, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> stepsize128 =  {0b00000101, 0b00000000, 0b11101100, 0b00010001, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> stepsize064 =  {0b00000101, 0b00000000, 0b11101100, 0b00010010, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> stepsize032 =  {0b00000101, 0b00000000, 0b11101100, 0b00010011, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> stepsize016 =  {0b00000101, 0b00000000, 0b11101100, 0b00010100, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> stepsize008 =  {0b00000101, 0b00000000, 0b11101100, 0b00010101, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> stepsize004 =  {0b00000101, 0b00000000, 0b11101100, 0b00010110, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> stepsize002 =  {0b00000101, 0b00000000, 0b11101100, 0b00010111, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> stepsize001 =  {0b00000101, 0b00000000, 0b11101100, 0b00011000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};

    // Deactivate and activate driver.  Note, same register as step size!                                        Driver on/off bit 1
  std::array<uint8_t, 8> driverOff =    {0b00000101, 0b00000000, 0b11101100, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> driverOn =     {0b00000101, 0b00000000, 0b11101100, 0b00010000, 0b00000000, 0b00000000, 0b00000010, 0b00000000};
  
  std::array<uint8_t, 8> tmcConfig;  // This is used to update the TMC driver configuration, especially to turn the driver on and off.

  // Velocity Dependent Control  0x10                                              32 bits data                                    CRC
  std::array<uint8_t, 8> iHoldiRun =          {0b00000101, 0b00000000, 0b10010000, 0b00000000, 0b00000001, 0b00011111, 0b00000000, 0b00000000};
  //                                                                                       IHOLDDELAY^^^^       ^^^^^IRUN   ^^^^^IHOLD_IRUN 
  //  Configuration of the power off behavior and braking.  PWMCONF Register 0x70.  Default is 0xC10D0024
  //                                               Sync        Slave addr  RW+Reg Addr 32 bits Data                                    CRC
  //  Default                                                                      0b11000001  0b11010000  0b00000000  0b00100100
  std::array<uint8_t, 8> powerBrakingConfig = {0b00000101, 0b00000000, 0b11110000, 0b11000001, 0b00111101, 0b00000000, 0b00100100, 0b00000000};
//                                                                                                     ^^FREEWHEEL 
TmcStepper();

uint8_t calcCRC(std::array<uint8_t, 8> datagram);

const uint8_t* getCommand(std::array<uint8_t, 8>& datagram);

const uint8_t* tmcDriverPower(bool driverPower);

};
