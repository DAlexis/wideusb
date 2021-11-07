
project(tcp-hub)


set(EXE_SOURCES
    main.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME} PRIVATE wideusb-host wideusb-common)

