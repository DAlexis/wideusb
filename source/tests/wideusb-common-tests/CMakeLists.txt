project("wideusb-common-unit-tests")

set(EXE_SOURCES
    ${PROJECT_SOURCE_DIR}/test-base64.cpp
    ${PROJECT_SOURCE_DIR}/test-nmea.cpp

)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries(${PROJECT_NAME} wideusb-modules)
target_link_libraries(${PROJECT_NAME} gtest_main)
