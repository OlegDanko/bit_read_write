cmake_minimum_required(VERSION 3.14)

project(bit_io LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(bit_io INTERFACE)

target_include_directories(bit_io INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
