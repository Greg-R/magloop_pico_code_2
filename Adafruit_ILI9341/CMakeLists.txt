add_library(ili9341 INTERFACE)

target_sources(ili9341 INTERFACE Adafruit_ILI9341.cpp Adafruit_ILI9341.h)

target_include_directories(ili9341 INTERFACE ../arduino_includes)

target_include_directories(ili9341 INTERFACE ../Adafruit-GFX-Library)

target_include_directories(ili9341 INTERFACE ../Adafruit_SPITFT)



