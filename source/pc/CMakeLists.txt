cmake_minimum_required(VERSION 3.18)

project(wideusb-pc-tools CXX)

add_subdirectory(wideusb-pc)
add_subdirectory(cli)
add_subdirectory(virtual-device)
add_subdirectory(tcp-hub)
add_subdirectory(python-bindings)
