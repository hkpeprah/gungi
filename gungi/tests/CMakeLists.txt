# Keep track of the test directory.
set (TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gungi)

# Main source file for the test runner.
set (TEST_RUNNER ${TEST_DIR}/main.cpp
                 ${TEST_DIR}/builder_unit_tests.cpp
                 ${TEST_DIR}/gndecoder_unit_tests.cpp
                 ${TEST_DIR}/gtypes_unit_tests.cpp
                 ${TEST_DIR}/logician_unit_tests.cpp
                 ${TEST_DIR}/player_unit_tests.cpp
                 ${TEST_DIR}/posn_unit_tests.cpp
                 ${TEST_DIR}/scenario_unit_tests.cpp
                 ${TEST_DIR}/tower_unit_tests.cpp
                 ${TEST_DIR}/util_unit_tests.cpp
                 ${TEST_DIR}/unit_unit_tests.cpp)

# List of the test source files.
set (TEST_FILES ${TEST_RUNNER})

# Add the test as an executable with the libraries and options to compile it
# with.
add_executable (gungi-test ${TEST_FILES})

# 'find_package ()' is necessary for the project to find 'CppUTest' which is a
# project instead of a library.
find_package (CppUTest REQUIRED)

# Specify the compile options and libraries to link to build the 'gungi' test
# code.
target_link_libraries (gungi-test gungi CppUTest CppUTestExt)
target_compile_options (gungi-test PRIVATE -Werror)

# Specify the include directory under which to find the private headers for the
# 'gungi' library.
target_include_directories(gungi-test PRIVATE ${PROJECT_DIR}/include)

# Specify the include directory under which to find the headers for the
# 'CppUTest' library.
target_include_directories (gungi-test PUBLIC ${ROOT_DIR}/submodules/cpputest/include)

# Compile the 'gtest' tests using the C++11 standard.
set_property (TARGET gungi-test PROPERTY CXX_STANDARD 11)

# Enable testing.  This lets 'cmake' know that there are tests that can be run,
# and what they are.
enable_testing ()
add_test (gungi gungi-test)
