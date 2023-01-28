
#pragma once
#include <stdint.h>
#include <array>



class TmcStepper {

public:

  // Reverse the stepper direction via UART.  This register also activates register for step size!
  // Set this register once at start-up.
  // GCONF register 0x0                  Sync        Slave Addr  RW+Reg Addr 32 bits Data                                    CRC
  std::array<uint8_t, 8> reverse =      {0b00000101, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b11001001, 0b00000000};
  std::array<uint8_t, 8> forward =      {0b00000101, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b11000001, 0b00000000};
  // Now adjust the step size.  Register 0x6C.                   Data
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

    // Deactivate driver.  Note, same register as step size!                                                     Driver on/off bit 1
  std::array<uint8_t, 8> driverOff =    {0b00000101, 0b00000000, 0b11101100, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  std::array<uint8_t, 8> driverOn =     {0b00000101, 0b00000000, 0b11101100, 0b00010000, 0b00000000, 0b00000000, 0b00000010, 0b00000000};
  
  std::array<uint8_t, 8> tmcConfig;  // This is used to update the TMC driver configuration, especially to turn the driver on and off.

TmcStepper();

uint8_t calcCRC(std::array<uint8_t, 8> datagram);

const uint8_t* getCommand(std::array<uint8_t, 8>& datagram);

const uint8_t* tmcDriverPower(bool driverPower);

};
