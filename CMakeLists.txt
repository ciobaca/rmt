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

add_test(RmtRunsSmt5 "./rmt" -v 0 ../tests/smt/smt5.rmt)
set_tests_properties (RmtRunsSmt4 PROPERTIES PASS_REGULAR_EXPRESSION "result is")

add_test(RmtRunsExample1 "./rmt" -v 0 ../examples/z-all-examples/example1.rmt)
set_tests_properties (RmtRunsExample1 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 2 solutions.")

add_test(RmtRunsExample2 "./rmt" -v 0 ../examples/z-all-examples/example2.rmt)
set_tests_properties (RmtRunsExample2 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 1 solutions.")

add_test(RmtRunsExample3 "./rmt" -v 0 ../examples/z-all-examples/example3.rmt)
set_tests_properties (RmtRunsExample3 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 4 solutions.")

add_test(RmtRunsExample4 "./rmt" -v 0 ../examples/z-all-examples/example4.rmt)
set_tests_properties (RmtRunsExample4 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")
set_tests_properties (RmtRunsExample4 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")

add_test(RmtRunsExample5 "./rmt" -v 0 ../examples/z-all-examples/example5.rmt)
set_tests_properties (RmtRunsExample5 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (RmtRunsExample5 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 not proved.")

add_test(RmtRunsExample6 "./rmt" -v 0 ../examples/z-all-examples/example6.rmt)
set_tests_properties (RmtRunsExample6 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (RmtRunsExample6 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")

