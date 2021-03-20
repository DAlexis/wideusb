cmake_minimum_required(VERSION 3.1)
project(pywideusb LANGUAGES CXX)

add_subdirectory(pybind11)
pybind11_add_module(example example.cpp)
