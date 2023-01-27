#include "TmcStepper.h"

TmcStepper::TmcStepper() {}

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


const uint8_t* TmcStepper::getCommand(std::array<uint8_t, 8>& datagram) 
{
datagram[7] = calcCRC(datagram);
return datagram.data();  // Get a C-style pointer to the first element of the std:array.
}
