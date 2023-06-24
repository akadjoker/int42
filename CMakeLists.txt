cmake_minimum_required(VERSION 3.10)  

project(compiler)             

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)

add_executable(compiler main9.cpp)   

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")


#add_custom_command(TARGET compiler  PRE_BUILD       COMMAND clear)

add_custom_command(TARGET compiler
                   POST_BUILD
                   COMMAND ../compiler)
