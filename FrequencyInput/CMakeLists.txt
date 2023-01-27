add_library(FrequencyInput INTERFACE)

target_sources(FrequencyInput INTERFACE FrequencyInput.cpp FrequencyInput.h)

target_include_directories(FrequencyInput INTERFACE ../Button ../Adafruit_ILI9341 ../arduino_includes ../DDS ../SWR ../AutoTune ../EEPROM ../GraphPlot ../Data ../Adafruit-GFX-Library/Fonts)
