cmake_minimum_required(VERSION 3.18)

project(wideusb-host CXX)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads)

set(LIB_SOURCES
    src/physical-layer-serial-port.cpp
    src/wideusb.cpp
    src/asio-utils.cpp

    src/tcp-hub.cpp
    src/physical-layer-asio.cpp
    src/physical-layer-tcp-client.cpp
)

set(LIB_INCLUDES
    wideusb/physical-layer-serial-port.hpp
    wideusb/wideusb.hpp
    wideusb/asio-utils.hpp

    wideusb/tcp-hub.hpp
    wideusb/physical-layer-asio.hpp
    wideusb/physical-layer-tcp-client.hpp
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${LIB_SOURCES} ${LIB_INCLUDES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC Boost::system Threads::Threads wideusb-common)

