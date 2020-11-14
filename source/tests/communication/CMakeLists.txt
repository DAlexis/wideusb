project("wideusb-communication-unit-tests")

set(EXE_SOURCES
    ${PROJECT_SOURCE_DIR}/test-ring-buffer.cpp
)

set(EXE_HPP
    
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES} ${EXE_HPP})

target_link_libraries(${PROJECT_NAME} wideusb-communication)
target_link_libraries(${PROJECT_NAME} gtest_main)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/include
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/cpp/tools
    ${PROJECT_SOURCE_DIR}/../../wideusb-firmware/c-tools/include
)

add_test(NAME stmod-testing COMMAND ${PROJECT_NAME})
