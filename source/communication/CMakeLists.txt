cmake_minimum_required(VERSION 3.8)

project("wideusb-communication-tools" C CXX)

add_subdirectory(lib)

if(NOT ARM_NONE_EABI_GCC_TOOLCHAIN)
if(GTEST_FOUND)
    # To enable ctest usage
    enable_testing()
    add_subdirectory(tests)
endif()
endif()
