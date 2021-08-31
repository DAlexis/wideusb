project("wideusb-firmware-unit-tests")

set(EXE_SOURCES
    test-base64.cpp
    test-nmea.cpp
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/src/modules/gps/nmea-parser.cpp
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/src/modules/gps/minmea.cpp
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/src/modules/gps/point.cpp
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/src/tools/base64.cpp
)

set(EXE_HPP
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/include/modules/gps/nmea-parser.hpp
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/include/modules/gps/minmea.h
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/include/tools/base64.hpp
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/include/host-communication.hpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES} ${EXE_HPP})

target_link_libraries(${PROJECT_NAME} wideusb-communication)
target_link_libraries(${PROJECT_NAME} gtest_main)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/include
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/rapidjson
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/tools
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/c-tools/include
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
