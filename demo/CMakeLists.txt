# Add executable called "demo" that is built from the source files that make up
# the demo application.
add_executable (demo src/main.cpp src/commandparser.cpp src/app.cpp)

# Compile the demo using the C++11 standard.
set_property (TARGET demo PROPERTY CXX_STANDARD 11)

# Link the executable to the "gungi" library.  Since the "gungi" library has
# public include directories, we will use those link directories when building
# the demo.
target_link_libraries (demo LINK_PUBLIC gungi)
