cmake_minimum_required(VERSION 3.8)

project("wideusb-all")

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(wideusb-common)

IF(ARM_NONE_EABI_GCC_TOOLCHAIN)
    add_subdirectory(stm32)
ELSE()
    add_subdirectory(pc)
    add_subdirectory(tests)
ENDIF()
