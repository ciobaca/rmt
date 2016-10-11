cmake_minimum_required(VERSION 2.8.9)
project(main)
file(GLOB SOURCES "*.cpp")
add_executable(main ${SOURCES})
