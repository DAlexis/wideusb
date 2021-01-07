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

    ./include/communication/communicator.hpp
    ./include/communication/message.hpp
    ./include/communication/receiver.hpp
    ./include/communication/sender.hpp
    ./include/communication/serialization.hpp
    ./include/communication/channel-layer.hpp
    ./include/communication/identification.hpp
    ./include/communication/utils.hpp
    ./include/communication/networking.hpp
    ./include/communication/networking-interfaces.hpp
    ./include/communication/json/streamificator.hpp
    ./include/communication/json/identificator.hpp
    ./include/communication/json/rapidjson-config.h
    ./include/communication/json/helpers-json.hpp
    ./include/communication/json/messages-core.hpp
    ./include/communication/binary/channel.hpp
    ./include/communication/binary/network.hpp
    ./include/communication/binary/transport.hpp
    ./include/buffer.hpp
    ./include/messages/core-messages.hpp
    ./include/messages/gps-messages.hpp
    ./include/transport.hpp


    ./src/communication/communicator.cpp
    ./src/communication/receiver.cpp
    ./src/communication/sender.cpp
    ./src/communication/utils.cpp
    ./src/communication/json/streamificator.cpp
    ./src/communication/json/identificator.cpp
    ./src/communication/json/messages-core.cpp
    ./src/communication/binary/channel.cpp
    ./src/communication/binary/network.cpp
    ./src/communication/binary/transport.cpp
    ./src/buffer.cpp
    ./src/communication/json/helpers-json.cpp
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/include
)


#IF(NOT ARM_NONE_EABI_GCC_TOOLCHAIN)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -DNDEBUG")
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

#endif()
