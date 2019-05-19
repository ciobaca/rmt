cmake_minimum_required(VERSION 2.8.9)
project(rmt C CXX)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

add_definitions ("-O3 -Wall -Wextra -std=c++14 -fno-pic")

# Make sure you pass the path using "/" even on Windows.
find_package(Z3
  REQUIRED
  CONFIG
  # `NO_DEFAULT_PATH` is set so that -DZ3_DIR has to be passed to find Z3.
  # This should prevent us from accidently picking up an installed
  # copy of Z3. This is here to benefit Z3's build sytem when building
  # this project. When making your own project you probably shouldn't
  # use this option.
  NO_DEFAULT_PATH
)
message(STATUS "Z3_FOUND: ${Z3_FOUND}")
message(STATUS "Found Z3 ${Z3_VERSION_STRING}")
message(STATUS "Z3_DIR: ${Z3_DIR}")

file(GLOB SOURCES "*.cpp")
file(GLOB_RECURSE INCS "*.h")

add_executable(rmt ${SOURCES} ${INCS})

add_subdirectory(fastterm)

target_include_directories(rmt PRIVATE ${Z3_C_INCLUDE_DIRS})
target_include_directories(rmt PRIVATE fastterm)
target_link_libraries(rmt PRIVATE ${Z3_LIBRARIES})
target_link_libraries(rmt PRIVATE fastterm)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "[Ww]indows")
  # On Windows we need to copy the Z3 libraries
  # into the same directory as the executable
  # so that they can be found.
  foreach (z3_lib ${Z3_LIBRARIES})
    message(STATUS "Adding copy rule for ${z3_lib}")
    add_custom_command(TARGET rmt
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${z3_lib}>
        $<TARGET_FILE_DIR:rmt>
    )
  endforeach()
endif()

install (TARGETS rmt DESTINATION bin)
