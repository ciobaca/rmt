cmake_minimum_required(VERSION 2.8.9)
project(rmt C CXX)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

add_definitions ("-O3 -Wall -Wextra -std=c++14 -fno-pic -fpermissive")

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
target_include_directories(fastrmt PRIVATE ../fastterm)
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
set_tests_properties (TestFastParsingSort5 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

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

add_test(TestFastParsingSubSort7 "../fastrmt" -v 0 ../../tests/parsing/subsort7.rmt)
set_tests_properties (TestFastParsingSubSort7 PROPERTIES PASS_REGULAR_EXPRESSION "Error: incorrect subsorting")

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
set_tests_properties (TestFastParsingSignature13 PROPERTIES PASS_REGULAR_EXPRESSION "Unknown builtin function asdf")

add_test(TestFastParsingBuiltins1 "../fastrmt" -v 0 ../../tests/parsing/builtins1.rmt)
set_tests_properties (TestFastParsingBuiltins1 PROPERTIES
PASS_REGULAR_EXPRESSION "2 solutions")

add_test(TestFastParsingVar1 "../fastrmt" -v 0 ../../tests/parsing/var1.rmt)
set_tests_properties (TestFastParsingVar1 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastParsingVar2 "../fastrmt" -v 0 ../../tests/parsing/var2.rmt)
set_tests_properties (TestFastParsingVar2 PROPERTIES PASS_REGULAR_EXPRESSION "variable already exists")

add_test(TestFastParsingVar3 "../fastrmt" -v 0 ../../tests/parsing/var3.rmt)
set_tests_properties (TestFastParsingVar3 PROPERTIES PASS_REGULAR_EXPRESSION "identifier already used")

add_test(TestFastParsingRewriteSystem1 "../fastrmt" -v 0 ../../tests/parsing/rewritesystem1.rmt)
set_tests_properties (TestFastParsingRewriteSystem1 PROPERTIES PASS_REGULAR_EXPRESSION "Done.")

add_test(TestFastBasicAbstract1 "../fastrmt" -v 0 ../../tests/basic/abstract1.rmt)
set_tests_properties (TestFastBasicAbstract1 PROPERTIES PASS_REGULAR_EXPRESSION "\\(loop N _"
		     			     		PASS_REGULAR_EXPRESSION "aa|-> \\(mplus S I\\) }")
# TODO: check why these do not work as expected

add_test(TestFastBasicAbstract2 "../fastrmt" -v 0 ../../tests/basic/abstract2.rmt)
set_tests_properties (TestFastBasicAbstract2 PROPERTIES PASS_REGULAR_EXPRESSION "Abstract term     =  \\(extra \\(loop N N\\) \\(init N\\)\\)"
                                                        PASS_REGULAR_EXPRESSION "Abstracting subst =  {  }")
# TODO: check why these do not work as expected

add_test(TestFastBasicUnif1 "../fastrmt" -v 0 ../../tests/basic/unif1.rmt)
set_tests_properties (TestFastBasicUnif1 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicUnif2 "../fastrmt" -v 0 ../../tests/basic/unif2.rmt)
set_tests_properties (TestFastBasicUnif2 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicUnif3 "../fastrmt" -v 0 ../../tests/basic/unif3.rmt)
set_tests_properties (TestFastBasicUnif3 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicUnif4 "../fastrmt" -v 0 ../../tests/basic/unif4.rmt)
set_tests_properties (TestFastBasicUnif4 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicUnif5 "../fastrmt" -v 0 ../../tests/basic/unif5.rmt)
set_tests_properties (TestFastBasicUnif5 PROPERTIES PASS_REGULAR_EXPRESSION "No solution.")

add_test(TestFastBasicUnif6 "../fastrmt" -v 0 ../../tests/basic/unif6.rmt)
set_tests_properties (TestFastBasicUnif6 PROPERTIES PASS_REGULAR_EXPRESSION "No solution.")

add_test(TestFastBasicUnif7 "../fastrmt" -v 0 ../../tests/basic/unif7.rmt)
set_tests_properties (TestFastBasicUnif7 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicUnif8 "../fastrmt" -v 0 ../../tests/basic/unif8.rmt)
set_tests_properties (TestFastBasicUnif8 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicUnif9 "../fastrmt" -v 0 ../../tests/basic/unif9.rmt)
set_tests_properties (TestFastBasicUnif9 PROPERTIES PASS_REGULAR_EXPRESSION "No solution.")

add_test(TestFastSmt1 "../fastrmt" -v 0 ../../tests/smt/smt1.rmt)
set_tests_properties (TestFastSmt1 PROPERTIES PASS_REGULAR_EXPRESSION "is SAT")

add_test(TestFastSmt2 "../fastrmt" -v 0 ../../tests/smt/smt2.rmt)
set_tests_properties (TestFastSmt2 PROPERTIES PASS_REGULAR_EXPRESSION "is UNSAT")

add_test(TestFastBasicSmtUnif1 "../fastrmt" -v 0 ../../tests/basic/smtunif1.rmt)
set_tests_properties (TestFastBasicSmtUnif1 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicSmtUnif2 "../fastrmt" -v 0 ../../tests/basic/smtunif2.rmt)
set_tests_properties (TestFastBasicSmtUnif2 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicSmtUnif3 "../fastrmt" -v 0 ../../tests/basic/smtunif3.rmt)
set_tests_properties (TestFastBasicSmtUnif3 PROPERTIES
PASS_REGULAR_EXPRESSION "No solution")

add_test(TestFastBasicSmtUnif4 "../fastrmt" -v 0 ../../tests/basic/smtunif4.rmt)
set_tests_properties (TestFastBasicSmtUnif4 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicSmtUnif5 "../fastrmt" -v 0 ../../tests/basic/smtunif5.rmt)
set_tests_properties (TestFastBasicSmtUnif5 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastBasicSmtUnif6 "../fastrmt" -v 0 ../../tests/basic/smtunif6.rmt)
set_tests_properties (TestFastBasicSmtUnif6 PROPERTIES PASS_REGULAR_EXPRESSION "No solution")

add_test(TestFastBasicSmtUnif7 "../fastrmt" -v 0 ../../tests/basic/smtunif7.rmt)
set_tests_properties (TestFastBasicSmtUnif7 PROPERTIES PASS_REGULAR_EXPRESSION "Solution count: 1")

add_test(TestFastSearchExample1 "../fastrmt" -v 0 ../../tests/search/search1.rmt)
set_tests_properties (TestFastSearchExample1 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 2 solutions.")

add_test(TestFastSearchExample2 "../fastrmt" -v 0 ../../tests/search/search2.rmt)
set_tests_properties (TestFastSearchExample2 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 1 solutions.")

add_test(TestFastSearchExample3 "../fastrmt" -v 0 ../../tests/search/search3.rmt)
set_tests_properties (TestFastSearchExample3 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 1 solutions.")

add_test(TestFastSearchExample4 "../fastrmt" -v 0 ../../tests/search/search4.rmt)
set_tests_properties (TestFastSearchExample4 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 0 solutions.")

add_test(TestFastSearchExample5 "../fastrmt" -v 0 ../../tests/search/search5.rmt)
set_tests_properties (TestFastSearchExample5 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 4 solutions.")

add_test(TestFastSearchExample6 "../fastrmt" -v 0 ../../tests/search/search6.rmt)
set_tests_properties (TestFastSearchExample6 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 2 solutions.")

add_test(TestFastSearchExample7 "../fastrmt" -v 0 ../../tests/search/search7.rmt)
set_tests_properties (TestFastSearchExample7 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 2 solutions.")

add_test(TestFastSearchExample8 "../fastrmt" -v 0 ../../tests/search/search8.rmt)
set_tests_properties (TestFastSearchExample8 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 2 solutions.")

add_test(TestFastSearchExample9 "../fastrmt" -v 0 ../../tests/search/search9.rmt)
set_tests_properties (TestFastSearchExample9 PROPERTIES
PASS_REGULAR_EXPRESSION "done 10")

add_test(TestFastReachExample1A "../fastrmt" -v 0 ../../tests/reach/reach1.rmt)
set_tests_properties (TestFastReachExample1A PROPERTIES
PASS_REGULAR_EXPRESSION "1 proved")

add_test(TestFastReachExample1B "../fastrmt" -v 0 ../../tests/reach/reach1.rmt)
set_tests_properties (TestFastReachExample1B PROPERTIES
PASS_REGULAR_EXPRESSION "2 proved")

add_test(TestFastReachExample2A "../fastrmt" -v 0 ../../tests/reach/reach2.rmt)
set_tests_properties (TestFastReachExample2A PROPERTIES
PASS_REGULAR_EXPRESSION "1 proved")

add_test(TestFastReachExample2B "../fastrmt" -v 0 ../../tests/reach/reach2.rmt)
set_tests_properties (TestFastReachExample2B PROPERTIES
PASS_REGULAR_EXPRESSION "2 not proved")

add_test(TestFastReachExample3ASlow "../fastrmt" -v 0 ../../tests/reach/reach3.rmt)
set_tests_properties (TestFastReachExample3ASlow PROPERTIES
PASS_REGULAR_EXPRESSION "1 proved")

add_test(TestFastReachExample3BSlow "../fastrmt" -v 0 ../../tests/reach/reach3.rmt)
set_tests_properties (TestFastReachExample3BSlow PROPERTIES
PASS_REGULAR_EXPRESSION "2 proved")

add_test(TestFastReachExample4A "../fastrmt" -v 0 ../../tests/reach/reach4.rmt)
set_tests_properties (TestFastReachExample4A PROPERTIES
PASS_REGULAR_EXPRESSION "1 proved")

add_test(TestFastReachExample4B "../fastrmt" -v 0 ../../tests/reach/reach4.rmt)
set_tests_properties (TestFastReachExample4B PROPERTIES
PASS_REGULAR_EXPRESSION "2 proved")

add_test(TestFastReachExample5A "../fastrmt" -v 0 ../../tests/reach/reach5.rmt)
set_tests_properties (TestFastReachExample5A PROPERTIES
PASS_REGULAR_EXPRESSION "1 proved")

add_test(TestFastReachExample5B "../fastrmt" -v 0 ../../tests/reach/reach5.rmt)
set_tests_properties (TestFastReachExample5B PROPERTIES
PASS_REGULAR_EXPRESSION "2 proved")

add_test(TestFastReachExample6A "../fastrmt" -v 0 ../../tests/reach/reach6.rmt)
set_tests_properties (TestFastReachExample6A PROPERTIES
PASS_REGULAR_EXPRESSION "1 proved")

add_test(TestFastReachExample6B "../fastrmt" -v 0 ../../tests/reach/reach6.rmt)
set_tests_properties (TestFastReachExample6B PROPERTIES
PASS_REGULAR_EXPRESSION "2 proved")

add_test(TestFastReachExample7A "../fastrmt" -v 0 ../../tests/reach/reach7.rmt)
set_tests_properties (TestFastReachExample7A PROPERTIES
PASS_REGULAR_EXPRESSION "1 proved")

add_test(TestFastReachExample7B "../fastrmt" -v 0 ../../tests/reach/reach7.rmt)
set_tests_properties (TestFastReachExample7B PROPERTIES
PASS_REGULAR_EXPRESSION "2 proved")

add_test(TestFastReachExample7C "../fastrmt" -v 0 ../../tests/reach/reach7.rmt)
set_tests_properties (TestFastReachExample7C PROPERTIES
PASS_REGULAR_EXPRESSION "3 proved")

add_test(TestFastReachExample8A "../fastrmt" -v 0 ../../tests/reach/reach8.rmt)
set_tests_properties (TestFastReachExample8A PROPERTIES
PASS_REGULAR_EXPRESSION "1 proved")

add_test(TestFastReachExample8B "../fastrmt" -v 0 ../../tests/reach/reach8.rmt)
set_tests_properties (TestFastReachExample8B PROPERTIES
PASS_REGULAR_EXPRESSION "2 proved")

add_test(TestFastReachExample8C "../fastrmt" -v 0 ../../tests/reach/reach8.rmt)
set_tests_properties (TestFastReachExample8C PROPERTIES
PASS_REGULAR_EXPRESSION "3 proved")

add_test(TestFastReachExample9 "../fastrmt" -v 0 ../../tests/reach/reach9.rmt)
set_tests_properties (TestFastReachExample9 PROPERTIES
PASS_REGULAR_EXPRESSION "1 not proved")

add_test(TestFastDefined1 "../fastrmt" -v 0 ../../tests/defined/defined1.rmt)
set_tests_properties (TestFastDefined1 PROPERTIES PASS_REGULAR_EXPRESSION "Done")

add_test(TestFastDefined2 "../fastrmt" -v 0 ../../tests/defined/defined2.rmt)
set_tests_properties (TestFastDefined2 PROPERTIES PASS_REGULAR_EXPRESSION "Result is")

add_test(TestFastDefined3 "../fastrmt" -v 0 ../../tests/defined/defined3.rmt)
set_tests_properties (TestFastDefined3 PROPERTIES PASS_REGULAR_EXPRESSION "Success: 2 solutions.")

