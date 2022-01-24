cmake_minimum_required(VERSION 3.18)

project(wideusb-pc CXX)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads)

set(LIB_SOURCES
    src/physical-layer-serial-port.cpp
    src/asio-utils.cpp
    src/asio-net-srv-runner.cpp

    src/tcp-hub.cpp
    src/physical-layer-asio.cpp
    src/physical-layer-tcp-client.cpp
    src/package-inspector.cpp
    src/socket-queue-mutex.cpp
)

set(LIB_INCLUDES
    wideusb-pc/physical-layer-serial-port.hpp
    wideusb-pc/asio-utils.hpp
    wideusb-pc/asio-net-srv-runner.hpp

    wideusb-pc/tcp-hub.hpp
    wideusb-pc/physical-layer-asio.hpp
    wideusb-pc/physical-layer-tcp-client.hpp
    wideusb-pc/package-inspector.hpp
    wideusb-pc/socket-queue-mutex.hpp
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${LIB_SOURCES} ${LIB_INCLUDES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC Boost::system Threads::Threads wideusb-common)

