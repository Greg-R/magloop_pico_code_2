# C++ Code for the Raspberry Pi Pico Magnetic Loop Antenna Controller

A "proof of concept" project to replace the STM32F103 "Blue Pill"
which is used in the "Controller for Double-Double Mag Loop" described in the book
"Microcontroller Projects for Amateur Radio by Jack Purdum, W8TEE, and
Albert Peter, AC8GY".

The STM32 was replaced with the Raspberry Pi Pico in this derivative project.

The project consists of:
 1.  Firmware for the Raspberry Pi Pico microcontroller.
 2.  A PCB design.
 3.  A "Bill of Material" list of required components.
 4.  Files for 3D printing of a case and various antenna components.
 5.  Detailed documentation covering construction and usage.

The firmware was developed using Visual Studio Code and the Raspberry Pi Pico SDK.
It is not compatible with the Arduino IDE, although a future fork to Arduino may be possible in the
future.

Copyright (C) 2022  Gregory Raven
