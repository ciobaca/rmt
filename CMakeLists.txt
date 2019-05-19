cmake_minimum_required(VERSION 2.8.9)
project(rmt)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(examples)

include(CTest)

add_test(TestSmt1 "./rmt" -v 0 ../tests/smt/smt1.rmt)
set_tests_properties (TestSmt1 PROPERTIES PASS_REGULAR_EXPRESSION "is SAT")

add_test(TestSmt2 "./rmt" -v 0 ../tests/smt/smt2.rmt)
set_tests_properties (TestSmt2 PROPERTIES PASS_REGULAR_EXPRESSION "is UNSAT")

add_test(TestSmt3 "./rmt" -v 0 ../tests/smt/smt3.rmt)
set_tests_properties (TestSmt3 PROPERTIES PASS_REGULAR_EXPRESSION "is false")

add_test(TestSmt4 "./rmt" -v 0 ../tests/smt/smt4.rmt)
set_tests_properties (TestSmt4 PROPERTIES PASS_REGULAR_EXPRESSION "is B")

add_test(TestSmt5 "./rmt" -v 0 ../tests/smt/smt5.rmt)
set_tests_properties (TestSmt5 PROPERTIES PASS_REGULAR_EXPRESSION "result is")

add_test(TestReachExample1 "./rmt" -v 0 ../examples/z-all-examples/example1.rmt)
set_tests_properties (TestReachExample1 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 2 solutions.")

add_test(TestReachExample2 "./rmt" -v 0 ../examples/z-all-examples/example2.rmt)
set_tests_properties (TestReachExample2 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 1 solutions.")

add_test(TestReachExample3 "./rmt" -v 0 ../examples/z-all-examples/example3.rmt)
set_tests_properties (TestReachExample3 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 4 solutions.")

add_test(TestReachExample4 "./rmt" -v 0 ../examples/z-all-examples/example4.rmt)
set_tests_properties (TestReachExample4 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")
set_tests_properties (TestReachExample4 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")

add_test(TestReachExample5 "./rmt" -v 0 ../examples/z-all-examples/example5.rmt)
set_tests_properties (TestReachExample5 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (TestReachExample5 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 not proved.")

add_test(TestReachExample6Slow "./rmt" -v 0 ../examples/z-all-examples/example6.rmt)
set_tests_properties (TestReachExample6Slow PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (TestReachExample6Slow PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")

add_test(TestReachExample7Slow "./rmt" -v 0 ../examples/z-all-examples/example7.rmt)
set_tests_properties (TestReachExample7Slow PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (TestReachExample7Slow PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")

add_test(TestReachExample8Slow "./rmt" -v 0 ../examples/z-all-examples/example8.rmt)
set_tests_properties (TestReachExample8Slow PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (TestReachExample8Slow PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")

add_test(TestReachExample9Slow "./rmt" -v 0 ../examples/z-all-examples/example9.rmt)
set_tests_properties (TestReachExample9Slow PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (TestReachExample9Slow PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")

add_test(TestReachExample10 "./rmt" -v 0 ../examples/z-all-examples/example10.rmt)
set_tests_properties (TestReachExample10 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (TestReachExample10 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")

add_test(TestReachExample11 "./rmt" -v 0 ../examples/z-all-examples/example11.rmt)
set_tests_properties (TestReachExample11 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (TestReachExample11 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")

add_test(TestReachExample12 "./rmt" -v 0 ../examples/z-all-examples/example12.rmt)
set_tests_properties (TestReachExample12 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (TestReachExample12 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")
set_tests_properties (TestReachExample12 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #3 proved.")

add_test(TestReachExample13 "./rmt" -v 0 ../examples/z-all-examples/example13.rmt)
set_tests_properties (TestReachExample13 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #1 proved.")
set_tests_properties (TestReachExample13 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #2 proved.")
set_tests_properties (TestReachExample13 PROPERTIES PASS_REGULAR_EXPRESSION "Circularity #3 proved.")

add_test(TestReachExample14 "./rmt" -v 0 ../examples/z-all-examples/example14.rmt)
set_tests_properties (TestReachExample14 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 2 solutions.")

add_test(TestDefined1 "./rmt" -v 0 ../tests/defined/defined1.rmt)
set_tests_properties (TestDefined1 PROPERTIES PASS_REGULAR_EXPRESSION "Succeeded in proving circularity #1")

add_test(TestDefined2 "./rmt" -v 0 ../tests/defined/defined2.rmt)
set_tests_properties (TestDefined2 PROPERTIES PASS_REGULAR_EXPRESSION "Result is")

add_test(TestEquiv1Slow "./rmt" -v 0 ../tests/equivalence/imp-running-example-1.rmt)
set_tests_properties (TestEquiv1Slow PROPERTIES PASS_REGULAR_EXPRESSION "Succeeded in proving circularity #1")
set_tests_properties (TestEquiv1Slow PROPERTIES PASS_REGULAR_EXPRESSION "Succeeded in proving circularity #2")
set_tests_properties (TestEquiv1Slow PROPERTIES PASS_REGULAR_EXPRESSION "Succeeded in proving circularity #3")

add_test(TestFastTerm1 "./testft1")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "t1 = i\\(x1\\)")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "t2 = f\\(x1,x2\\)")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "t3 = f\\(i\\(x1\\),f\\(x1,x2\\)\\)")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "t4 = e")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "t5 = g\\(i\\(x1\\),f\\(x1,x2\\),e\\)")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "0\\. term = and\\(bx1,not\\(bx2\\)\\)")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "1\\. sat = 1")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "2\\. sat = -1")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "3\\. sat = 1\\.")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "4\\. sat = -1\\.")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "5\\. sat = 1\\.")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match i\\(x1\\) and f\\(x1,x2\\): 0")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match i\\(x1\\) and f\\(x1,x2\\): 0")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match f\\(x1,x2\\) and f\\(x3,x4\\): 1")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match f\\(x3,x4\\) and f\\(x3,e\\): 0")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match f\\(x3,e\\) and f\\(x3,x3\\): 0")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match f\\(x3,x3\\) and f\\(e,i\\(x1\\)\\): 0")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match f\\(x1,x2\\) and i\\(x1\\): 0")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match f\\(x3,x4\\) and f\\(x1,x2\\): 1")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match f\\(x3,e\\) and f\\(x3,x4\\): 1")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match f\\(x3,x3\\) and f\\(x3,e\\): 0")
set_tests_properties (TestFastTerm1 PROPERTIES PASS_REGULAR_EXPRESSION "match f\\(e,i\\(x1\\)\\) and f\\(x3,x3\\): 0")
