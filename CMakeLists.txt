# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# If you used the install script, the PICO_SDK_PATH environment variable is set.
#set(PICO_SDK_PATH "/home/your_path")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(magloop_pico C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(magloop_pico magloop_pico.cpp)

pico_set_program_name(magloop_pico "magloop_pico")
pico_set_program_version(magloop_pico "0.1")

pico_enable_stdio_uart(magloop_pico 1)
pico_enable_stdio_usb(magloop_pico 0)

add_subdirectory(Adafruit_ILI9341)
add_subdirectory(Adafruit-GFX-Library)
add_subdirectory(arduino_includes)
add_subdirectory(Adafruit_SPITFT)
add_subdirectory(AccelStepper)
add_subdirectory(StepperManagement)
add_subdirectory(DDS)
add_subdirectory(SWR)
add_subdirectory(DisplayManagement)
add_subdirectory(DisplayUtility)
add_subdirectory(TuneInputs)
add_subdirectory(Button)
add_subdirectory(Rotary)
add_subdirectory(EEPROM)
add_subdirectory(GraphPlot)
add_subdirectory(Data)
add_subdirectory(TmcStepper)

# Add any user requested libraries
target_link_libraries(magloop_pico
        pico_stdlib
        hardware_spi
        hardware_timer
        hardware_clocks
        hardware_adc
        hardware_flash
        hardware_sync
        ili9341
        gfx
        spitft
        print
        stepper
        steppermanagement
        dds
        SWR
        DisplayManagement
        DisplayUtility
        TuneInputs
        Button
        Rotary
        EEPROM
        GraphPlot
        Data
        TmcStepper
        )

pico_add_extra_outputs(magloop_pico)

