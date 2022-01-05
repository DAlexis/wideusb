project("wideusb-common-unit-tests")

set(EXE_SOURCES
    test-base64.cpp
    test-nmea.cpp
    test-caching-set.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries(${PROJECT_NAME} wideusb-common)
target_link_libraries(${PROJECT_NAME} gtest_main)
