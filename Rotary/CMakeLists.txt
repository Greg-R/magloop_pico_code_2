add_library(Rotary INTERFACE)

target_sources(Rotary INTERFACE Rotary.cpp Rotary.h)

target_include_directories(Rotary INTERFACE ../arduino_includes)

