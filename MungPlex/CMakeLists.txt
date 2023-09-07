# CMakeList.txt : CMake project for MungPlex, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.16)
project(MungPlex VERSION 2.0)

# Find lua
find_package(Lua REQUIRED)

# Find sol2
find_package(sol2 CONFIG REQUIRED)

# Find glfw
find_package(glfw3 CONFIG REQUIRED)

# Find nlohmann-json
find_package(nlohmann_json CONFIG REQUIRED)

# Generate MungPlexConfig.h using MungPlexConfig.h.in
configure_file(MungPlexConfig.h.in ${CMAKE_BINARY_DIR}/MungPlexConfig.h @ONLY)

# Add the main executable target
add_executable (MungPlex
    main.cpp
    Settings.h
    Settings.cpp
    Connection.cpp
    Connection.h
    ProcessInformation.cpp
    ProcessInformation.h
    Search.h
    Search.cpp
    Log.h
    Log.cpp
    PointerSearch.h
    PointerSearch.cpp
    HelperFunctions.h
    Cheats.h
    Cheats.cpp
    MemoryViewer.h
    MemoryViewer.cpp
    ../include/imgui/imgui.cpp
    ../include/imgui/imgui_demo.cpp
    ../include/imgui/imgui_draw.cpp
    ../include/imgui/imgui_tables.cpp
    ../include/imgui/imgui_widgets.cpp
    ../include/imgui/backends/imgui_impl_opengl3.cpp
    ../include/imgui/backends/imgui_impl_glfw.cpp
    ../include/MemoryCompare/MemCompareOperations.h
    ../include/MemoryCompare/MemCompareResult.h
    ../include/MemoryCompare/MemCompareResult.cpp
    ../include/MemoryCompare/MemCompare.h
    ../include/MemoryCompare/MemCompare.cpp
    ../include/MemoryCompare/MemDump.h
    ../include/MemoryCompare/MemDump.cpp
    ../include/MemoryCompare/include/LitColor/LitColor/LitColor.h
    ../include/MemoryCompare/include/MorphText/MorphText/MorphText.h
    ../include/MemoryCompare/include/OperativeArray/OperativeArray/OperativeArray.h
    ../include/Xertz/Xertz.h
    ../include/Xertz/MemoryRegion.h
    ../include/Xertz/MemoryRegion.cpp
    ../include/Xertz/ProcessInfo.h
    ../include/Xertz/ProcessInfo.cpp
    ../include/Xertz/SystemInfo.h
    ../include/Xertz/SystemInfo.cpp
    ../include/ImGuiFileDialog/ImGuiFileDialog.h
    ../include/ImGuiFileDialog/ImGuiFileDialog.cpp
    ../include/ImGui_MemoryEditor/imgui_memory_editor/imgui_memory_editor.h
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