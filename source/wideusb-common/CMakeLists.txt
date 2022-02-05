cmake_minimum_required(VERSION 3.1.0)

project("wideusb-common" C CXX)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    wideusb/front/base-front.hpp
    wideusb/front/dac-front.hpp
    wideusb/front/gps-front.hpp
    wideusb/front/monitor-front.hpp
    wideusb/front/discovery.hpp

    wideusb/back/base-back.hpp
    wideusb/back/dac-back.hpp
    wideusb/back/gps-back.hpp
    wideusb/back/monitor-back.hpp


    wideusb/utils/gps/minmea.h
    wideusb/utils/gps/nmea-parser.hpp
    wideusb/utils/gps/point.hpp
    wideusb/utils/base64.hpp
    wideusb/utils/utilities.hpp
    wideusb/utils/caching-set.hpp

    wideusb/device-core.hpp

    wideusb/buffer.hpp

    wideusb/communication/utils.hpp
    wideusb/communication/net-srv-runner.hpp
    wideusb/communication/networking.hpp
    wideusb/communication/socket.hpp
    wideusb/communication/socket-queue.hpp
    wideusb/communication/network-types.hpp
    wideusb/communication/network-interface.hpp
    wideusb/communication/i-channel-layer.hpp
    wideusb/communication/i-network-layer.hpp
    wideusb/communication/i-physical-layer.hpp
    wideusb/communication/i-transport-layer.hpp
    wideusb/communication/i-package-inspector.hpp
    wideusb/communication/binary/channel.hpp
    wideusb/communication/binary/network.hpp
    wideusb/communication/binary/transport.hpp
    wideusb/communication/modules/monitor.hpp
    wideusb/communication/modules/core.hpp
    wideusb/communication/modules/ports.hpp
    wideusb/communication/modules/ids.hpp

    wideusb/modules/monitor-types.hpp


    src/front/base-front.cpp
    src/front/dac-front.cpp
    src/front/gps-front.cpp
    src/front/monitor-front.cpp
    src/front/discovery.cpp

    src/back/dac-back.cpp
    src/back/gps-back.cpp
    src/back/monitor-back.cpp

    src/utils/utilities.cpp
    src/utils/gps/minmea.cpp
    src/utils/gps/nmea-parser.cpp
    src/utils/gps/point.cpp
    src/utils/base64.cpp

    src/device-core.cpp

    src/communication/networking.cpp
    src/communication/socket.cpp
    src/communication/utils.cpp
    src/communication/physical-layer.cpp
    src/communication/binary/channel.cpp
    src/communication/binary/network.cpp
    src/communication/binary/transport.cpp
    src/communication/utils/time-group-maker.cpp

    src/buffer.cpp
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}
)

IF(NOT ARM_NONE_EABI_GCC_TOOLCHAIN)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})


