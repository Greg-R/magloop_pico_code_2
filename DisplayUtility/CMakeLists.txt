add_library(DisplayUtility INTERFACE)

target_sources(DisplayUtility INTERFACE DisplayUtility.cpp DisplayUtility.h)

target_include_directories(DisplayUtility INTERFACE ../Button ../Adafruit_ILI9341 ../arduino_includes ../DDS ../SWR ../AutoTune ../EEPROM ../GraphPlot ../Data ../Rotary ../TuneInputs ../Adafruit-GFX-Library/Fonts) 
