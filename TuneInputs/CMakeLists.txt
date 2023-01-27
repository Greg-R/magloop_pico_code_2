add_library(TuneInputs INTERFACE)

target_sources(TuneInputs INTERFACE TuneInputs.cpp TuneInputs.h)

target_include_directories(TuneInputs INTERFACE ../Button ../Adafruit_ILI9341 ../arduino_includes ../DDS ../SWR ../AutoTune ../EEPROM ../GraphPlot ../Data ../DisplayUtility ../Adafruit-GFX-Library/Fonts)
