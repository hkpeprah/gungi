# Keep track of the project directory.
set (PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# List the source files that make up the "gungi" library.
set (SOURCE_FILES ${PROJECT_DIR}/src/builder.cpp
                  ${PROJECT_DIR}/src/gndecoder.cpp
                  ${PROJECT_DIR}/src/gtypes.cpp
                  ${PROJECT_DIR}/src/logician.cpp
                  ${PROJECT_DIR}/src/player.cpp
                  ${PROJECT_DIR}/src/posn.cpp
                  ${PROJECT_DIR}/src/tower.cpp
                  ${PROJECT_DIR}/src/unit.cpp
                  ${PROJECT_DIR}/src/util.cpp)

# Create a library called "gungi" which includes the source files that make up
# the "gungi" project.
add_library (gungi ${SOURCE_FILES})

# Compile the 'gungi' library using the C++11 standard.
set_property (TARGET gungi PROPERTY CXX_STANDARD 11)

# Specify the include directories so that the source files internal to our
# "gungi" library can find their respective headers.
target_include_directories (gungi PRIVATE ${PROJECT_DIR}/include)
target_compile_options (gungi PRIVATE -Werror)

# Make sure the compiler can find the include files for our "gungi" library
# when other libraries or executables link to "gungi".
target_include_directories (gungi INTERFACE ${PROJECT_DIR}/lib)
target_compile_options (gungi INTERFACE -Werror)

# Add the test subdirectory so that the tests can be compiled.
add_subdirectory (tests)
