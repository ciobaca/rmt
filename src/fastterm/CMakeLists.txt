cmake_minimum_required(VERSION 2.8.9)
project(testft C CXX)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

add_definitions ("-O3 -Wall -Wextra -std=c++14 -fno-pic -DNDEBUG")

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
message(STATUS "Z3_DIR: ${Z3_C_INCLUDE_DIRS}")

file(GLOB SOURCES1 "*.cpp")
message(STATUS "SOURCES1 (w/ main): ${SOURCES1}")
 foreach(SRCFILE IN LISTS SOURCES1)
    string(REGEX MATCH "^.*testft.*$" RESULT ${SRCFILE})
    if ("${RESULT}" STREQUAL "")
      list(APPEND SOURCES2 ${SRCFILE})
    endif()
endforeach()
message(STATUS "SOURCES2 (no main): ${SOURCES2}")

add_library(fastterm ${SOURCES2})

target_include_directories(fastterm PUBLIC ${Z3_C_INCLUDE_DIRS})

target_link_libraries(fastterm PRIVATE ${Z3_LIBRARIES})

file(GLOB_RECURSE INCS "*.h")

add_executable(testft1 ${SOURCES2} "testft1.cpp" ${INCS})
add_executable(testft2 ${SOURCES2} "testft2.cpp" ${INCS})

target_include_directories(testft1 PRIVATE ${Z3_C_INCLUDE_DIRS})
target_include_directories(testft2 PRIVATE fastterm)

target_link_libraries(testft1 PRIVATE ${Z3_LIBRARIES})
target_link_libraries(testft2 PRIVATE fastterm)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "[Ww]indows")
  # On Windows we need to copy the Z3 libraries
  # into the same directory as the executable
  # so that they can be found.
  foreach (z3_lib ${Z3_LIBRARIES})
    message(STATUS "Adding copy rule for ${z3_lib}")
    add_custom_command(TARGET testft1
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${z3_lib}>
        $<TARGET_FILE_DIR:testft1>
    )
    add_custom_command(TARGET testft2
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${z3_lib}>
        $<TARGET_FILE_DIR:testft2>
    )
  endforeach()
endif()

install (TARGETS testft1 DESTINATION bin)
install (TARGETS testft2 DESTINATION bin)
