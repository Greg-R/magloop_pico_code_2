add_library(steppermanagement INTERFACE)

target_sources(steppermanagement INTERFACE StepperManagement.cpp StepperManagement.h)

target_link_libraries(steppermanagement INTERFACE stepper)

target_include_directories(steppermanagement INTERFACE ../AccelStepper ../Button ../Data)
