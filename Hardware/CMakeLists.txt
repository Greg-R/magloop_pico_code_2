add_library(Hardware INTERFACE)

target_sources(Hardware INTERFACE Hardware.cpp Hardware.h)

target_include_directories(Hardware INTERFACE ../Button ../Adafruit_ILI9341 ../arduino_includes ../DDS ../SWR ../AutoTune ../EEPROM ../GraphPlot ../DisplayUtility ../Data ../TuneInputs ../Adafruit-GFX-Library/Fonts) 