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

# Generate MungPlexConfig.hpp using MungPlexConfig.hpp.in
configure_file(MungPlexConfig.hpp.in ${CMAKE_BINARY_DIR}/MungPlexConfig.hpp @ONLY)

# Add the main executable target
add_executable (MungPlex
    main.cpp
    Settings.hpp
    Settings.cpp
    Connection.cpp
    Connection.hpp
    ProcessInformation.cpp
    ProcessInformation.hpp
    Search.hpp
    Search.cpp
    Log.hpp
    Log.cpp
    PointerSearch.hpp
    PointerSearch.cpp
    HelperFunctions.hpp
    Cheats.hpp
    Cheats.cpp
    MemoryViewer.hpp
    MemoryViewer.cpp
    DataConversion.hpp
    DataConversion.cpp
    CheatConvert.hpp
    CheatConvert.cpp
    WatchControl.cpp
    WatchControl.hpp
    resources.rc
    ../include/imgui/imgui.cpp
    ../include/imgui/imgui_demo.cpp
    ../include/imgui/imgui_draw.cpp
    ../include/imgui/imgui_tables.cpp
    ../include/imgui/imgui_widgets.cpp
    ../include/imgui/backends/imgui_impl_opengl3.cpp
    ../include/imgui/backends/imgui_impl_glfw.cpp
    ../include/MemoryCompare/MemoryCompareOperators.hpp
    ../include/MemoryCompare/MemoryCompareResult.hpp
    ../include/MemoryCompare/MemoryCompareResult.cpp
    ../include/MemoryCompare/MemoryCompare.hpp
    ../include/MemoryCompare/MemoryCompare.cpp
    ../include/MemoryCompare/MemoryDump.hpp
    ../include/MemoryCompare/MemoryDump.cpp
    ../include/MemoryCompare/include/LitColor/LitColor/LitColor.hpp
    ../include/MemoryCompare/include/MorphText/MorphText/Lookup.hpp
    ../include/MemoryCompare/include/MorphText/MorphText/MorphText.hpp
    ../include/MemoryCompare/include/MorphText/MorphText/MorphText.cpp
    ../include/MemoryCompare/include/OperativeArray/OperativeArray/OperativeArray.hpp
    ../include/Xertz/Xertz.hpp
    ../include/Xertz/MemoryRegion.hpp
    ../include/Xertz/MemoryRegion.cpp
    ../include/Xertz/ProcessInfo.hpp
    ../include/Xertz/ProcessInfo.cpp
    ../include/Xertz/SystemInfo.hpp
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