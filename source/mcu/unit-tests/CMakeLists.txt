cmake_minimum_required(VERSION 3.1.0)

project("wideusb-tests-on-pc" C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -pg -g -lprofiler -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -lprofiler -DNDEBUG")

include(detect-gtest.cmake)

if(GTEST_FOUND)
    # To enable ctest usage
    enable_testing()
    add_subdirectory(tests)
endif()

