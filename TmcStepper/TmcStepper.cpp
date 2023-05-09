#include "TmcStepper.h"

//  A configuration Class for the TMC stepper driver.
TmcStepper::TmcStepper() {
    tmcConfig = {0, 0, 0, 0, 0, 0, 0, 0};
      // Set main configuration.  Set the TMC driver off.
  uart_write_blocking(uart1, getCommand(forward), 8);
  // Set the tmcConfig variable to the desired step size.  Driver is off by default.
  //tmcstepper.tmcConfig = tmcstepper.stepsize128;
  //this->tmcstepper.tmcConfig = this->tmcstepper.stepsize256;
  //this->tmcstepper.tmcConfig = this->tmcstepper.stepsize128;
  //this->tmcstepper.tmcConfig = this->tmcstepper.stepsize064;
  tmcConfig = stepsize016;
  //this->tmcstepper.tmcConfig = this->tmcstepper.stepsize008;
  // Set the step size and turn the driver off.
  uart_write_blocking(uart1, tmcDriverPower(false), 8);
  // Set the power off behavior and braking.
  uart_write_blocking(uart1, getCommand(powerBrakingConfig), 8);
  uart_write_blocking(uart1, getCommand(iHoldiRun), 8);
}

// Calculate the CRC, which is the last byte in the datagram.  Return the CRC.
uint8_t TmcStepper::calcCRC(std::array<uint8_t, 8> datagram) // Shouldn't datagramLength be fixed to 8?
{
    // void swuart_calcCRC(UCHAR* datagram, UCHAR datagramLength)

    int i, j;
    // uint8_t& crc = datagram + (datagramLength-1); // CRC located in last byte of message.  This is the pointer to the datagram.
    uint8_t crc = 0;
    uint8_t currentByte;
    // crc = 0;  //  Initialize CRC to zero.

    for (i = 0; i < 7; i++)
    {                              // Execute for all bytes of a message.  Should always be 7 bytes?
        currentByte = datagram[i]; // Retrieve a byte to be sent from Array

        for (j = 0; j < 8; j++)
        { // This loop goes through byte bit by bit.

            if ((crc >> 7) ^ (currentByte & 0x01)) // update CRC based result of XOR operation
            {
                crc = (crc << 1) ^ 0x07;
            }
            else
            {
                crc = (crc << 1);
            }
            currentByte = currentByte >> 1;
        } // for CRC bit

    } // for message byte

    return crc;
}

// This is required due to the way the driver uses a CRC datagram for error correction.
const uint8_t *TmcStepper::getCommand(std::array<uint8_t, 8> &datagram)
{
    datagram[7] = calcCRC(datagram);
    return datagram.data(); // Get a C-style pointer to the first element of the std:array (C++ feature).
}

// A function is needed which turns the driver on/off but does not disturb the step size.
// The current step size should always be written to tmcConfig.
// This function will modify the bits of the array for either driver on or driver off.

const uint8_t *TmcStepper::tmcDriverPower(bool driverPower)
{
    if (driverPower)
        tmcConfig[6] = 0b00000010;
    else
        tmcConfig[6] = 0b00000000;
    tmcConfig[7] = calcCRC(tmcConfig);
    return tmcConfig.data(); // Get a C-style pointer to the first element of the std:array (C++ feature).
}
