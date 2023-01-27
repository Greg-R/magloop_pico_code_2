add_library(SWR INTERFACE)

target_sources(SWR INTERFACE SWR.cpp SWR.h)

target_include_directories(SWR INTERFACE ../arduino_includes ../StepperManagement ../Adafruit_ILI9341 ../SWR)

