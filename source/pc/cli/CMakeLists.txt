#set (CMAKE_CXX_STANDARD 14)

project(wideusb-cli)


set(EXE_SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME} wideusb-common wideusb-pc)

