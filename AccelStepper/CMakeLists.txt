add_library(stepper INTERFACE)

target_sources(stepper INTERFACE AccelStepper.cpp AccelStepper.h)

target_include_directories(stepper INTERFACE ../arduino_includes)