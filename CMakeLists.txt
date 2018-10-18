cmake_minimum_required(VERSION 2.8.9)
project(rmt)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(examples)

include(CTest)

add_test(RmtRunsSmt1 "./rmt" -v 0 ../tests/smt/smt1.rmt)
set_tests_properties (RmtRunsSmt1 PROPERTIES PASS_REGULAR_EXPRESSION "is SAT")

add_test(RmtRunsSmt2 "./rmt" -v 0 ../tests/smt/smt2.rmt)
set_tests_properties (RmtRunsSmt2 PROPERTIES PASS_REGULAR_EXPRESSION "is UNSAT")

add_test(RmtRunsSmt3 "./rmt" -v 0 ../tests/smt/smt3.rmt)
set_tests_properties (RmtRunsSmt3 PROPERTIES PASS_REGULAR_EXPRESSION "is false")

add_test(RmtRunsSmt4 "./rmt" -v 0 ../tests/smt/smt4.rmt)
set_tests_properties (RmtRunsSmt4 PROPERTIES PASS_REGULAR_EXPRESSION "is B")
