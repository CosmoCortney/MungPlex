# CMakeList.txt : CMake project for MungPlex, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.16)
project(MungPlex VERSION 2.0)

# Find lua
find_package(Lua REQUIRED)

# Find sol2
find_package(sol2 CONFIG REQUIRED)


# Generate MungPlexConfig.h using MungPlexConfig.h.in
configure_file(MungPlexConfig.h.in ${CMAKE_BINARY_DIR}/MungPlexConfig.h @ONLY)

# Add the main executable target
add_executable (MungPlex
    main.cpp
    Connection.cpp
    Connection.h
    ProcessInformation.cpp
    ProcessInformation.h
    Search.cpp
    HelperFunctions.h
    Cheats.h
    Cheats.cpp
    ../include/imgui/imgui.cpp
    ../include/imgui/imgui_demo.cpp
    ../include/imgui/imgui_draw.cpp
    ../include/imgui/imgui_tables.cpp
    ../include/imgui/imgui_widgets.cpp
    ../include/imgui/backends/imgui_impl_opengl3.cpp
    ../include/imgui/backends/imgui_impl_glfw.cpp
    ../include/Xertz/MemDump.cpp
    ../include/Xertz/Xertz.h
    ../include/Xertz/MemCompareResult.h
    ../include/Xertz/MemoryRegion.cpp
    ../include/Xertz/ProcessInfo.cpp
    ../include/Xertz/SystemInfo.cpp
    #$<TARGET_OBJECTS:MainLibrary>  # Link with the MainLibrary target
)

# Link against lua
target_link_libraries(MungPlex ${LUA_LIBRARIES})

#add_library(MainLibrary OBJECT main.cpp)

# Add the main executable target
#add_executable (MungPlex ${MAIN_SOURCE_FILES})

#if (CMAKE_VERSION VERSION_GREATER 3.12)
 # set_property(TARGET MungPlex PROPERTY CXX_STANDARD 23)
 # set(CMAKE_CXX_STANDARD 20)
#endif()

find_package(OpenGL REQUIRED)

# Link libraries to the main target
target_link_libraries(MungPlex
    #MainLibrary
    glfw
    OpenGL::GL
    lua
    sol2
)

# Set C++ version
set_target_properties(MungPlex PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Set GLFW build options
set (GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set (GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set (GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)



# Include the binary directory to access the generated MungPlexConfig.h
target_include_directories(MungPlex PRIVATE ${CMAKE_BINARY_DIR})

# Install target
install(TARGETS MungPlex DESTINATION "MungPlex/bin")