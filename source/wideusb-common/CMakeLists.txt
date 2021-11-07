cmake_minimum_required(VERSION 3.1.0)

project("wideusb-common" C CXX)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    wideusb-common/front/base.hpp
    wideusb-common/front/dac-front.hpp
    wideusb-common/front/gps-front.hpp
    wideusb-common/front/monitor-front.hpp

    wideusb-common/back/dac-back.hpp
    wideusb-common/back/gps-back.hpp
    wideusb-common/back/monitor-back.hpp

    wideusb-common/utils/point.hpp
    wideusb-common/utils/utilities.hpp
    wideusb-common/utils/gps/minmea.h
    wideusb-common/utils/gps/nmea-parser.hpp
    wideusb-common/utils/base64.hpp

    wideusb-common/core-base.hpp

    wideusb-common/buffer.hpp

    wideusb-common/communication/utils.hpp
    wideusb-common/communication/networking.hpp
    wideusb-common/communication/network-types.hpp
    wideusb-common/communication/i-channel-layer.hpp
    wideusb-common/communication/i-network-layer.hpp
    wideusb-common/communication/i-physical-layer.hpp
    wideusb-common/communication/i-transport-layer.hpp
    wideusb-common/communication/binary/channel.hpp
    wideusb-common/communication/binary/network.hpp
    wideusb-common/communication/binary/transport.hpp
    wideusb-common/communication/modules/monitor.hpp
    wideusb-common/communication/modules/core.hpp
    wideusb-common/communication/modules/ports.hpp
    wideusb-common/communication/modules/ids.hpp
    wideusb-common/communication/utils/package-inspector.hpp


    src/front/base.cpp
    src/front/dac-front.cpp
    src/front/gps-front.cpp
    src/front/monitor-front.cpp

    src/back/dac-back.cpp
    src/back/gps-back.cpp
    src/back/monitor-back.cpp

    src/utils/point.cpp
    src/utils/utilities.cpp
    src/utils/gps/minmea.cpp
    src/utils/gps/nmea-parser.cpp
    src/utils/base64.cpp

    src/core-base.cpp

    src/communication/networking.cpp
    src/communication/utils.cpp
    src/communication/physical-layer.cpp
    src/communication/binary/channel.cpp
    src/communication/binary/network.cpp
    src/communication/binary/transport.cpp
    src/communication/utils/time-group-maker.cpp
    src/communication/utils/package-inspector.cpp

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


