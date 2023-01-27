
add_library(gfx INTERFACE)

target_sources(gfx INTERFACE Adafruit_GFX.cpp Adafruit_GFX.h)

target_include_directories(gfx INTERFACE ../arduino_includes)

