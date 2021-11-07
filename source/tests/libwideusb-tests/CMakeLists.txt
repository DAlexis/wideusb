project("libwideusb-unit-tests" CXX)

set(EXE_SOURCES
    ${PROJECT_SOURCE_DIR}/test-synchronizator.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries(${PROJECT_NAME} wideusb-host)
target_link_libraries(${PROJECT_NAME} gtest_main)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
