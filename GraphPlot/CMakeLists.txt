add_library(GraphPlot INTERFACE)

target_sources(GraphPlot INTERFACE GraphPlot.cpp GraphPlot.h)

target_include_directories(GraphPlot INTERFACE ../Adafruit_ILI9341 ../arduino_includes ../DDS ../Data)