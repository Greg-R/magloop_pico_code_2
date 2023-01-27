
add_library(spitft INTERFACE)

target_sources(spitft INTERFACE Adafruit_SPITFT.cpp Adafruit_SPITFT.h)

target_link_libraries(spitft INTERFACE gfx)

target_include_directories(spitft INTERFACE 
                           ../arduino_includes
                           )
