cmake_minimum_required(VERSION 3.18)

project(wideusb CXX)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads)

set(LIB_SOURCES
    ${PROJECT_SOURCE_DIR}/src/serial-port-physical-layer.cpp
    ${PROJECT_SOURCE_DIR}/src/wideusb.cpp
    ${PROJECT_SOURCE_DIR}/src/asio-utils.cpp
    ${PROJECT_SOURCE_DIR}/src/host-modules/host-module.cpp
    ${PROJECT_SOURCE_DIR}/src/host-modules/monitor.cpp
    ${PROJECT_SOURCE_DIR}/src/host-modules/gps.cpp
)

set(LIB_INCLUDES
    ${PROJECT_SOURCE_DIR}/include/serial-port-physical-layer.hpp
    ${PROJECT_SOURCE_DIR}/include/wideusb.hpp
    ${PROJECT_SOURCE_DIR}/include/asio-utils.hpp
    ${PROJECT_SOURCE_DIR}/include/host-modules/host-module.hpp
    ${PROJECT_SOURCE_DIR}/include/host-modules/monitor.hpp
    ${PROJECT_SOURCE_DIR}/include/host-modules/gps.hpp
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${LIB_SOURCES} ${LIB_INCLUDES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC Boost::system Threads::Threads wideusb-communication)

