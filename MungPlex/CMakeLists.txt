# CMakeList.txt : CMake project for MungPlex, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.16)
project(MungPlex VERSION 2.0)

# Set the Lua source and binary directories
set(LUA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/include/lua")
set(LUA_BINARY_DIR "${CMAKE_BINARY_DIR}/lua")
set(SOL2_DIR "${CMAKE_SOURCE_DIR}/include/sol2")
set(SOL2_BINARY_DIR "${CMAKE_BINARY_DIR}/sol2")

# Build the Lua library
add_subdirectory(${LUA_SOURCE_DIR} ${LUA_BINARY_DIR})

# Build the Sol2 library
add_subdirectory(${SOL2_DIR} ${SOL2_BINARY_DIR})

# Exclude lua.c from the Lua library build
set_target_properties(lua PROPERTIES
    EXCLUDE_FROM_ALL 1
    EXCLUDE_FROM_DEFAULT_BUILD 1
)

# Create a separate Lua library
add_library(lualib STATIC ${LUA_SOURCE_DIR}/lua.c)

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
    #${LUA_SOURCE_DIR}/lauxlib.c
    #${LUA_SOURCE_DIR}/lbaselib.c
    #${LUA_SOURCE_DIR}/lcode.c
   # ${LUA_SOURCE_DIR}/lcorolib.c
    #${LUA_SOURCE_DIR}/lctype.c
  #  ${LUA_SOURCE_DIR}/ldblib.c
   # ${LUA_SOURCE_DIR}/ldebug.c
 #   ${LUA_SOURCE_DIR}/ldo.c
#    ${LUA_SOURCE_DIR}/ldump.c
 #   ${LUA_SOURCE_DIR}/lfunc.c
  #  ${LUA_SOURCE_DIR}/lgc.c
 #   ${LUA_SOURCE_DIR}/linit.c
  #  ${LUA_SOURCE_DIR}/liolib.c
  #  ${LUA_SOURCE_DIR}/llex.c
  #  ${LUA_SOURCE_DIR}/lmathlib.c
  #  ${LUA_SOURCE_DIR}/lmem.c
  #  ${LUA_SOURCE_DIR}/loadlib.c
  #  ${LUA_SOURCE_DIR}/lobject.c
   # ${LUA_SOURCE_DIR}/lopcodes.c
 #   ${LUA_SOURCE_DIR}/loslib.c
 #   ${LUA_SOURCE_DIR}/lparser.c
  #  ${LUA_SOURCE_DIR}/lstate.c
   # ${LUA_SOURCE_DIR}/lstring.c
 #   ${LUA_SOURCE_DIR}/lstrlib.c
 #   ${LUA_SOURCE_DIR}/ltable.c
  #  ${LUA_SOURCE_DIR}/ltablib.c
   # ${LUA_SOURCE_DIR}/ltm.c
  #  ${LUA_SOURCE_DIR}/lua.c
  #  ${LUA_SOURCE_DIR}/lundump.c
  #  ${LUA_SOURCE_DIR}/lutf8lib.c
  #  ${LUA_SOURCE_DIR}/lvm.c
  #  ${LUA_SOURCE_DIR}/lzio.c
    ../include/sol2/include/sol/sol.hpp
    #$<TARGET_OBJECTS:MainLibrary>  # Link with the MainLibrary target
)

#add_library(MainLibrary OBJECT main.cpp)

# Add the main executable target
#add_executable (MungPlex ${MAIN_SOURCE_FILES})

# Link Sol2 library
#target_link_libraries(MungPlex PRIVATE ${SOL2_DIR}/lib/sol2.lib)

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

#target_include_directories(lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
