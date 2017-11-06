cmake_minimum_required(VERSION 2.8.9)
project(rmt)

file(GLOB SOURCES "*.cpp")
file(GLOB_RECURSE INCS "*.h")
add_definitions ("-Wall -Wextra -std=c++11")
add_executable(rmt ${SOURCES} ${INCS})
