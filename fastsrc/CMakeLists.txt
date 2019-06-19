cmake_minimum_required(VERSION 2.8.9)
project(rmt C CXX)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

add_definitions ("-O3 -Wall -Wextra -std=c++14 -fno-pic -DNDEBUG")

# set (Gperftools_DIR "${CMAKE_CURRENT_LIST_DIR}")
# find_package(Gperftools REQUIRED)

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

add_executable(fastrmt ${SOURCES} ${INCS})

# target_link_libraries(fastrmt PRIVATE ${GPERFTOOLS_TCMALLOC} ${GPERFTOOLS_PROFILER})

# add_subdirectory(fastterm)

target_include_directories(fastrmt PRIVATE ${Z3_C_INCLUDE_DIRS})
target_include_directories(fastrmt PRIVATE ../src/fastterm)
target_link_libraries(fastrmt PRIVATE ${Z3_LIBRARIES})
target_link_libraries(fastrmt PRIVATE fastterm)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "[Ww]indows")
  # On Windows we need to copy the Z3 libraries
  # into the same directory as the executable
  # so that they can be found.
  foreach (z3_lib ${Z3_LIBRARIES})
    message(STATUS "Adding copy rule for ${z3_lib}")
    add_custom_command(TARGET fastrmt
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${z3_lib}>
        $<TARGET_FILE_DIR:fastrmt>
    )
  endforeach()
endif()

install (TARGETS fastrmt DESTINATION bin)

include(CTest)

add_test(TestFastParsingSort1 "../fastrmt" -v 0 ../../tests/parsing/dupsort1.rmt)
set_tests_properties (TestFastParsingSort1 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastParsingSort2 "../fastrmt" -v 0 ../../tests/parsing/dupsort2.rmt)
set_tests_properties (TestFastParsingSort2 PROPERTIES PASS_REGULAR_EXPRESSION "sort already exists")

add_test(TestFastParsingSort3 "../fastrmt" -v 0 ../../tests/parsing/dupsort3.rmt)
set_tests_properties (TestFastParsingSort3 PROPERTIES PASS_REGULAR_EXPRESSION "sort already exists")

add_test(TestFastParsingSort4 "../fastrmt" -v 0 ../../tests/parsing/dupsort4.rmt)
set_tests_properties (TestFastParsingSort4 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastParsingSort5 "../fastrmt" -v 0 ../../tests/parsing/interpretedsort1.rmt)
set_tests_properties (TestFastParsingSort5 PROPERTIES PASS_REGULAR_EXPRESSION "todo: cannot handle interpreted sorts yet")

add_test(TestFastParsingSubSort1 "../fastrmt" -v 0 ../../tests/parsing/subsort1.rmt)
set_tests_properties (TestFastParsingSubSort1 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastParsingSubSort2 "../fastrmt" -v 0 ../../tests/parsing/subsort2.rmt)
set_tests_properties (TestFastParsingSubSort2 PROPERTIES PASS_REGULAR_EXPRESSION "Error: cannot add subsorting \\(would create cycle\\)")

add_test(TestFastParsingSubSort3 "../fastrmt" -v 0 ../../tests/parsing/subsort3.rmt)
set_tests_properties (TestFastParsingSubSort3 PROPERTIES PASS_REGULAR_EXPRESSION "Error: cannot add subsorting \\(would create cycle\\)")

add_test(TestFastParsingSubSort4 "../fastrmt" -v 0 ../../tests/parsing/subsort4.rmt)
set_tests_properties (TestFastParsingSubSort4 PROPERTIES PASS_REGULAR_EXPRESSION "Error: cannot add subsorting to builtin sorts")

add_test(TestFastParsingSubSort5 "../fastrmt" -v 0 ../../tests/parsing/subsort5.rmt)
set_tests_properties (TestFastParsingSubSort5 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastParsingSubSort6 "../fastrmt" -v 0 ../../tests/parsing/subsort6.rmt)
set_tests_properties (TestFastParsingSubSort6 PROPERTIES PASS_REGULAR_EXPRESSION "Error: cannot add subsorting to builtin sorts")

add_test(TestFastParsingSignature1 "../fastrmt" -v 0 ../../tests/parsing/signature1.rmt)
set_tests_properties (TestFastParsingSignature1 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastParsingSignature2 "../fastrmt" -v 0 ../../tests/parsing/signature2.rmt)
set_tests_properties (TestFastParsingSignature2 PROPERTIES PASS_REGULAR_EXPRESSION "Error: duplicate function symbol")

add_test(TestFastParsingSignature3 "../fastrmt" -v 0 ../../tests/parsing/signature3.rmt)
set_tests_properties (TestFastParsingSignature3 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastParsingSignature4 "../fastrmt" -v 0 ../../tests/parsing/signature4.rmt)
set_tests_properties (TestFastParsingSignature4 PROPERTIES PASS_REGULAR_EXPRESSION "Unity element of ACU function must be nullary")

add_test(TestFastParsingSignature5 "../fastrmt" -v 0 ../../tests/parsing/signature5.rmt)
set_tests_properties (TestFastParsingSignature5 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastParsingSignature6 "../fastrmt" -v 0 ../../tests/parsing/signature6.rmt)
set_tests_properties (TestFastParsingSignature6 PROPERTIES PASS_REGULAR_EXPRESSION "Unsupported combination of a/c/u properties")

add_test(TestFastParsingSignature7 "../fastrmt" -v 0 ../../tests/parsing/signature7.rmt)
set_tests_properties (TestFastParsingSignature7 PROPERTIES PASS_REGULAR_EXPRESSION "Unsupported combination of a/c/u properties")

add_test(TestFastParsingSignature8 "../fastrmt" -v 0 ../../tests/parsing/signature8.rmt)
set_tests_properties (TestFastParsingSignature8 PROPERTIES PASS_REGULAR_EXPRESSION "Unsupported combination of a/c/u properties")

add_test(TestFastParsingSignature9 "../fastrmt" -v 0 ../../tests/parsing/signature9.rmt)
set_tests_properties (TestFastParsingSignature9 PROPERTIES PASS_REGULAR_EXPRESSION "AC functions must be defined on the same sort")

add_test(TestFastParsingSignature10 "../fastrmt" -v 0 ../../tests/parsing/signature10.rmt)
set_tests_properties (TestFastParsingSignature10 PROPERTIES PASS_REGULAR_EXPRESSION "AC functions must be defined on the same sort")

add_test(TestFastParsingSignature11 "../fastrmt" -v 0 ../../tests/parsing/signature11.rmt)
set_tests_properties (TestFastParsingSignature11 PROPERTIES PASS_REGULAR_EXPRESSION "AC functions must be defined on the same sort")

add_test(TestFastParsingSignature12 "../fastrmt" -v 0 ../../tests/parsing/signature12.rmt)
set_tests_properties (TestFastParsingSignature12 PROPERTIES PASS_REGULAR_EXPRESSION "Associative or commutative functions must have exactly two arguments")

add_test(TestFastParsingSignature13 "../fastrmt" -v 0 ../../tests/parsing/signature13.rmt)
set_tests_properties (TestFastParsingSignature13 PROPERTIES PASS_REGULAR_EXPRESSION "todo: cannot handle interpreted functions yet")

add_test(TestFastParsingVar1 "../fastrmt" -v 0 ../../tests/parsing/var1.rmt)
set_tests_properties (TestFastParsingVar1 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastParsingVar2 "../fastrmt" -v 0 ../../tests/parsing/var2.rmt)
set_tests_properties (TestFastParsingVar2 PROPERTIES PASS_REGULAR_EXPRESSION "variable already exists")

add_test(TestFastParsingVar3 "../fastrmt" -v 0 ../../tests/parsing/var3.rmt)
set_tests_properties (TestFastParsingVar3 PROPERTIES PASS_REGULAR_EXPRESSION "identifier already used")
