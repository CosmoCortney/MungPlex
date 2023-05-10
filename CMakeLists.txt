# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("MungPlex")
# Include sub-projects.
add_subdirectory ("MungPlex")
include_directories("include/imgui")
add_subdirectory ("include/glfw")
include_directories ("include/glad")
include_directories ("include/meojson")
add_subdirectory ("include/Xertz")
add_subdirectory ("include/OperativeArray")

