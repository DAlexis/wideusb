cmake_minimum_required(VERSION 3.1.0)

project("wideusb-communication" C CXX)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    ./include/rapidjson/prettywriter.h
    ./include/rapidjson/rapidjson.h
    ./include/rapidjson/document.h
    ./include/rapidjson/ostreamwrapper.h
    ./include/rapidjson/cursorstreamwrapper.h
    ./include/rapidjson/encodings.h
    ./include/rapidjson/filewritestream.h
    ./include/rapidjson/schema.h
    ./include/rapidjson/filereadstream.h
    ./include/rapidjson/reader.h
    ./include/rapidjson/memorybuffer.h
    ./include/rapidjson/stringbuffer.h
    ./include/rapidjson/memorystream.h
    ./include/rapidjson/pointer.h
    ./include/rapidjson/error/error.h
    ./include/rapidjson/error/en.h
    ./include/rapidjson/istreamwrapper.h
    ./include/rapidjson/msinttypes/stdint.h
    ./include/rapidjson/msinttypes/inttypes.h
    ./include/rapidjson/writer.h
    ./include/rapidjson/allocators.h
    ./include/rapidjson/fwd.h
    ./include/rapidjson/encodedstream.h
    ./include/rapidjson/stream.h
    ./include/rapidjson/internal/swap.h
    ./include/rapidjson/internal/biginteger.h
    ./include/rapidjson/internal/ieee754.h
    ./include/rapidjson/internal/regex.h
    ./include/rapidjson/internal/itoa.h
    ./include/rapidjson/internal/pow10.h
    ./include/rapidjson/internal/meta.h
    ./include/rapidjson/internal/clzll.h
    ./include/rapidjson/internal/diyfp.h
    ./include/rapidjson/internal/strfunc.h
    ./include/rapidjson/internal/stack.h
    ./include/rapidjson/internal/strtod.h
    ./include/rapidjson/internal/dtoa.h
    ./include/host-listener.hpp
    ./include/rapidjson-config.h
    ./include/ring-buffer.h
    
    ./src/host-listener.cpp
    ./src/ring-buffer.c
    
    #PARENT_SCOPE
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/include
    #PARENT_SCOPE
)


#IF(NOT ARM_NONE_EABI_GCC_TOOLCHAIN)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -pg -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -DNDEBUG")
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

#endif()
