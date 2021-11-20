cmake_minimum_required(VERSION 3.1)
project(pywideusb LANGUAGES CXX)

add_subdirectory(3rdparty/pybind11)

SET(MODULE_SOURCES
    pywideusb.cpp
    py-buffer.hpp
    py-buffer.cpp
    py-physical-layer.hpp
    py-physical-layer.cpp
    py-wideusb-device.cpp
    py-wideusb-device.hpp
    py-net-service.hpp
    py-net-service.cpp
    py-monitor.cpp
    py-monitor.hpp
    py-gps.cpp
    py-gps.hpp
    py-dac.cpp
    py-dac.hpp
    py-asio-utils.hpp
    py-asio-utils.cpp
)

add_library(${PROJECT_NAME} MODULE ${MODULE_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC pybind11::module wideusb-host wideusb-common)

configure_file(tests/test-dac.py test-dac.py COPYONLY)

set(RESOURCE_FILES
    tests/test-dac.py
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RESOURCE "${RESOURCE_FILES}"
)

pybind11_strip(${PROJECT_NAME})
pybind11_extension(${PROJECT_NAME})

#pybind11_add_module(${PROJECT_NAME} pywideusb.cpp)
