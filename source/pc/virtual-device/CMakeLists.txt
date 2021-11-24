project(wideusb-virtual-device CXX)

set(EXE_SOURCES
    src/main.cpp
    src/virtual-device.cpp
    src/modules/virtual-monitor-impl.cpp
    src/modules/virtual-dac-impl.cpp

    include/virtual-device.hpp
    include/modules/virtual-monitor-impl.hpp
    include/modules/virtual-dac-impl.hpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries (${PROJECT_NAME} wideusb-pc)
target_link_libraries (${PROJECT_NAME} wideusb-common)
