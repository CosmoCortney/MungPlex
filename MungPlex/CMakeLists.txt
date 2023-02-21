# CMakeList.txt : CMake project for MungPlex, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (MungPlex "main.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MungPlex PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
