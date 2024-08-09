# CMakeList.txt : CMake project for MungPlex, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.16)
project(MungPlex VERSION 2.5.0)

find_package(Lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread filesystem iostreams)
find_library(CPP_GAME_SDK discord_game_sdk_cpp)
find_library(DISCORD_GAME_SDK_LIB discord_game_sdk)
find_library(DISCORD_GAME_SDK_CPP_LIB discord_game_sdk_cpp)

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
    DataConversion.h
    DataConversion.cpp
    CheatConvert.h
    CheatConvert.cpp
    WatchControl.cpp
    WatchControl.h
    resources.rc
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
    ../include/MemoryCompare/include/MorphText/MorphText/Lookup.h
    ../include/MemoryCompare/include/MorphText/MorphText/MorphText.h
    ../include/MemoryCompare/include/MorphText/MorphText/MorphText.cpp
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

# Link Icon
target_sources(MungPlex PRIVATE resources.rc)

find_package(OpenGL REQUIRED)

# Link libraries to the main target
target_link_libraries(MungPlex PRIVATE
    glfw
    OpenGL::GL
    sol2
    ${LUA_LIBRARIES}
    ${CPP_GAME_SDK}
    ${DISCORD_GAME_SDK_LIB}
    ${DISCORD_GAME_SDK_CPP_LIB}
    ${LUA_LIBRARIES}
    ${Boost_LIBRARIES}
    Boost::filesystem
    Boost::iostreams
)

# Set C++ version
set_target_properties(MungPlex PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_include_directories(MungPlex PRIVATE 
    ${CMAKE_BINARY_DIR}
    ${LUA_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)

# Install target
install(TARGETS MungPlex DESTINATION "MungPlex/bin")