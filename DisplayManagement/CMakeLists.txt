add_library(DisplayManagement INTERFACE)

target_sources(DisplayManagement INTERFACE DisplayManagement.cpp DisplayManagement.h)

target_include_directories(DisplayManagement INTERFACE ../Button ../Adafruit_ILI9341 ../arduino_includes ../DDS ../SWR ../AutoTune ../EEPROM ../GraphPlot ../DisplayUtility ../Data ../TuneInputs ../Adafruit-GFX-Library/Fonts) 
