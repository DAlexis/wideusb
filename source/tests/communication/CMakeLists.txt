project("wideusb-communication-unit-tests")

set(EXE_SOURCES
    ${PROJECT_SOURCE_DIR}/test-ring-buffer.cpp
    ${PROJECT_SOURCE_DIR}/test-buffer.cpp
    ${PROJECT_SOURCE_DIR}/test-channel-layer-binary.cpp
    ${PROJECT_SOURCE_DIR}/test-network-layer-binary.cpp
    ${PROJECT_SOURCE_DIR}/test-transport-layer-binary.cpp
    ${PROJECT_SOURCE_DIR}/test-networking.cpp
    ${PROJECT_SOURCE_DIR}/test-grouper.cpp
)

set(EXE_HPP
    
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES} ${EXE_HPP})

target_link_libraries(${PROJECT_NAME} wideusb-communication)
target_link_libraries(${PROJECT_NAME} gtest_main)

target_include_directories(${PROJECT_NAME} PRIVATE

)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
