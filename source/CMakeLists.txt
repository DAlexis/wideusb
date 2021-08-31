cmake_minimum_required(VERSION 3.8)

project("wideusb-all")

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(communication/lib)
add_subdirectory(wideusb-common)

IF(ARM_NONE_EABI_GCC_TOOLCHAIN)
    add_subdirectory(wideusb-firmware)
ELSE()
    add_subdirectory(host)
    add_subdirectory(tests)
    add_subdirectory(python-bindings)
ENDIF()
