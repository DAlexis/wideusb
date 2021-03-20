cmake_minimum_required(VERSION 3.1.0)

project("wideusb-communication" C CXX)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    ./include/communication/utils.hpp
    ./include/communication/networking.hpp
    ./include/communication/network-types.hpp
    ./include/communication/i-channel-layer.hpp
    ./include/communication/i-network-layer.hpp
    ./include/communication/i-physical-layer.hpp
    ./include/communication/i-transport-layer.hpp
    ./include/communication/binary/channel.hpp
    ./include/communication/binary/network.hpp
    ./include/communication/binary/transport.hpp
    ./include/communication/modules/monitor.hpp
    ./include/communication/modules/core.hpp
    ./include/communication/modules/ports.hpp
    ./include/communication/modules/ids.hpp

    ./include/buffer.hpp


    ./src/communication/networking.cpp
    ./src/communication/utils.cpp
    ./src/communication/physical-layer.cpp
    ./src/communication/binary/channel.cpp
    ./src/communication/binary/network.cpp
    ./src/communication/binary/transport.cpp
    ./src/communication/utils/time-group-maker.cpp
    ./src/buffer.cpp
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/include
)


IF(NOT ARM_NONE_EABI_GCC_TOOLCHAIN)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -DNDEBUG")
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

#
