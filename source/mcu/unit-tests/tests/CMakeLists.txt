project("wideusb-unit-tests")

set(EXE_SOURCES
    ${PROJECT_SOURCE_DIR}/test-base64.cpp
    ${PROJECT_SOURCE_DIR}/test-json-input.cpp
    ${PROJECT_SOURCE_DIR}/../../wideusb/cpp/src/tools/base64.cpp
    ${PROJECT_SOURCE_DIR}/../../wideusb/cpp/src/tools/ring-buffer-json.cpp
)

set(EXE_HPP
    ${PROJECT_SOURCE_DIR}/../../wideusb/cpp/include/tools/base64.hpp
    ${PROJECT_SOURCE_DIR}/../../wideusb/cpp/include/tools/ring-buffer-json.hpp
    ${PROJECT_SOURCE_DIR}/../../wideusb/cpp/include/host-communication.hpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES} ${EXE_HPP})

target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/../../wideusb/cpp/include/
    ${PROJECT_SOURCE_DIR}/../../wideusb/cpp/rapidjson/
    ${PROJECT_SOURCE_DIR}/../../wideusb/cpp/tools/
)

add_test(NAME stmod-testing COMMAND ${PROJECT_NAME})
