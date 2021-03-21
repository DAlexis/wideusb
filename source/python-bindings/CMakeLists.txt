cmake_minimum_required(VERSION 3.1)
project(pywideusb LANGUAGES CXX)

add_subdirectory(3rdparty/pybind11)

SET(MODULE_SOURCES
    pywideusb.cpp
    py-wideusb-device.cpp
    py-wideusb-device.hpp
    py-monitor.cpp
    py-monitor.hpp
    py-gps.cpp
    py-gps.hpp
)

add_library(${PROJECT_NAME} MODULE ${MODULE_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC pybind11::module wideusb)

pybind11_strip(${PROJECT_NAME})
pybind11_extension(${PROJECT_NAME})

#pybind11_add_module(${PROJECT_NAME} pywideusb.cpp)
