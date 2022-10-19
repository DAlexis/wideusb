project("wideusb-communication-unit-tests")

set(EXE_SOURCES
    test-ring-buffer.cpp
    test-buffer.cpp
    test-channel-layer-binary.cpp
    test-network-layer-binary.cpp
    test-transport-layer-binary.cpp
    test-networking.cpp
    test-networking-chain.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES} ${EXE_HPP})

target_link_libraries(${PROJECT_NAME} wideusb-common wideusb-pc)
target_link_libraries(${PROJECT_NAME} gtest_main)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
