project("wideusb-pc-unit-tests" CXX)

set(EXE_SOURCES
    ${PROJECT_SOURCE_DIR}/test-synchronizator.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries(${PROJECT_NAME} wideusb-pc)
target_link_libraries(${PROJECT_NAME} gtest_main)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
