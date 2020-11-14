cmake_minimum_required(VERSION 3.8)

project("wideusb-all")


add_subdirectory(communication)

IF(ARM_NONE_EABI_GCC_TOOLCHAIN)
    add_subdirectory(wideusb-firmware)
ELSE()
    add_subdirectory(tests)
ENDIF()
