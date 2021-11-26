cmake_minimum_required(VERSION 3.1)
project(pywideusb LANGUAGES CXX)

add_subdirectory(3rdparty/pybind11)

SET(MODULE_SOURCES
    src/pywideusb.cpp
    src/py-buffer.cpp
    src/py-physical-layer.cpp
    src/py-net-service.cpp
    src/py-monitor.cpp
    src/py-gps.cpp
    src/py-dac.cpp
    src/py-asio-utils.cpp

    include/py-buffer.hpp
    include/py-physical-layer.hpp
    include/py-net-service.hpp
    include/py-monitor.hpp
    include/py-gps.hpp
    include/py-dac.hpp
    include/py-asio-utils.hpp

)

add_library(${PROJECT_NAME} MODULE ${MODULE_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC pybind11::module wideusb-pc wideusb-common)
target_include_directories(${PROJECT_NAME} PUBLIC include)

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
