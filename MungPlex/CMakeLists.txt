# CMakeList.txt : CMake project for MungPlex, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.16)
project(MungPlex VERSION 2.6.0)

find_package(Lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread filesystem iostreams)
find_library(CPP_GAME_SDK discord_game_sdk_cpp)
find_library(DISCORD_GAME_SDK_LIB discord_game_sdk)
find_library(DISCORD_GAME_SDK_CPP_LIB discord_game_sdk_cpp)
find_library(FTDI_LIB NAMES ftdi PATHS "${FT_LIB_FOLDER}/amd64")

# Generate MungPlexConfig.hpp using MungPlexConfig.hpp.in
configure_file(MungPlexConfig.hpp.in ${CMAKE_BINARY_DIR}/MungPlexConfig.hpp @ONLY)

# Add the main executable target
add_executable (MungPlex
    source/Cheats/CheatConvert.cpp
    source/Cheats/CheatConvert.hpp
    source/Cheats/Cheats.cpp
    source/Cheats/Cheats.hpp
    source/CheatSearch/Search.hpp
    source/CheatSearch/Search.cpp
    source/Connection/Connection.cpp
    source/Connection/Connection.hpp
    source/Connection/Discord.cpp
    source/Connection/Discord.hpp
    source/Connection/USBGecko.hpp
    source/Connection/USBGecko.cpp
    source/Connection/ConsoleConnectionWrappers/IConsoleConnectionWrapper.cpp
    source/Connection/ConsoleConnectionWrappers/IConsoleConnectionWrapper.hpp
    source/Connection/ConsoleConnectionWrappers/USBGeckoConnectionWrapper_Impl.cpp
    source/Connection/ConsoleConnectionWrappers/USBGeckoConnectionWrapper_Impl.hpp
    source/ContextMenu/ContextMenu.cpp
    source/ContextMenu/ContextMenu.hpp
    source/DataConversion/DataConversion.hpp
    source/DataConversion/DataConversion.cpp
    source/DeviceControl/DeviceControl.cpp
    source/DeviceControl/DeviceControl.hpp
    source/DeviceControl/DeviceLovense_Impl.cpp
    source/DeviceControl/DeviceLovense_Impl.hpp
    source/DeviceControl/IDevice.cpp
    source/DeviceControl/IDevice.hpp
    source/DeviceControl/LovenseToy.cpp
    source/DeviceControl/LovenseToy.hpp
    source/Helpers/BigNHelpers.hpp
    source/Helpers/HelperFunctions.hpp
    source/Helpers/ResourcesHelpers.hpp
    source/Helpers/Pairs.cpp
    source/Helpers/Pairs.hpp
    #source/Helpers/WidgetHelpers.cpp
    #source/Helpers/WidgetHelpers.hpp
    Log.hpp
    Log.cpp
    main.cpp
    source/MemoryViewer/MemoryViewer.hpp
    source/MemoryViewer/MemoryViewer.cpp
    source/PointerSearch/PointerSearch.hpp
    source/PointerSearch/PointerSearch.cpp
    source/ProcessInformation/ProcessInformation.cpp
    source/ProcessInformation/ProcessInformation.hpp
    source/ProcessInformation/Emulators/Cemu_Impl.cpp
    source/ProcessInformation/Emulators/Cemu_Impl.hpp
    source/ProcessInformation/Emulators/Dolphin_Impl.cpp
    source/ProcessInformation/Emulators/Dolphin_Impl.hpp
    source/ProcessInformation/Emulators/Fusion_Impl.cpp
    source/ProcessInformation/Emulators/Fusion_Impl.hpp
    source/ProcessInformation/Emulators/IEmulator.cpp
    source/ProcessInformation/Emulators/IEmulator.hpp
    source/ProcessInformation/Emulators/Lime3DS_Impl.cpp
    source/ProcessInformation/Emulators/Lime3DS_Impl.hpp
    source/ProcessInformation/Emulators/MelonDS_Impl.cpp
    source/ProcessInformation/Emulators/MelonDS_Impl.hpp
    source/ProcessInformation/Emulators/Mesen_Impl.cpp
    source/ProcessInformation/Emulators/Mesen_Impl.hpp
    source/ProcessInformation/Emulators/mGBA_Impl.cpp
    source/ProcessInformation/Emulators/mGBA_Impl.hpp
    source/ProcessInformation/Emulators/No$PSX_Impl.cpp
    source/ProcessInformation/Emulators/No$PSX_Impl.hpp
    source/ProcessInformation/Emulators/PCSX2_Impl.cpp
    source/ProcessInformation/Emulators/PCSX2_Impl.hpp
    source/ProcessInformation/Emulators/PPSSPP_Impl.cpp
    source/ProcessInformation/Emulators/PPSSPP_Impl.hpp
    source/ProcessInformation/Emulators/Project64_Impl.cpp
    source/ProcessInformation/Emulators/Project64_Impl.hpp
    source/ProcessInformation/Emulators/RPCS3_Impl.cpp
    source/ProcessInformation/Emulators/RPCS3_Impl.hpp
    source/ProcessInformation/Emulators/Yuzu_Impl.cpp
    source/ProcessInformation/Emulators/Yuzu_Impl.hpp
    resources.rc
    source/Settings/Settings.hpp
    source/Settings/Settings.cpp
    source/WatchControl/BoolView_Impl.cpp
    source/WatchControl/BoolView_Impl.hpp
    source/WatchControl/FloatView_Impl.cpp
    source/WatchControl/FloatView_Impl.hpp
    source/WatchControl/IntegralView_Impl.cpp
    source/WatchControl/IntegralView_Impl.hpp
    source/WatchControl/IView.cpp
    source/WatchControl/IView.hpp
    source/WatchControl/MousePianoView_Impl.cpp
    source/WatchControl/MousePianoView_Impl.hpp
    source/WatchControl/WatchControl.cpp
    source/WatchControl/WatchControl.hpp
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
    ${LOVENSE_SDK_FOLDER}/WinSDK_Release/includes/Lovense.h
    ${LOVENSE_SDK_FOLDER}/WinSDK_Release/includes/LovenseDef.h
    ${LOVENSE_SDK_FOLDER}/WinSDK_Release/includes/LovenseToyInfo.h
    ${FT_LIB_FOLDER}/ftd2xx.h
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
    kernel32 user32 gdi32
    ${FT_LIB_FOLDER}/amd64/ftd2xx.lib
)

set(LOVENSE_LIB_DIR "${LOVENSE_SDK_FOLDER}/WinSDK_Release/x64")
set(OUT_DIR "${CMAKE_SOURCE_DIR}/out/build")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(MungPlex PRIVATE ${LOVENSE_LIB_DIR}/Debug/LovenseToys.lib)
    add_custom_command(TARGET MungPlex POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LOVENSE_LIB_DIR}/Debug/LovenseToys.dll
        ${OUT_DIR}/x64-Debug/MungPlex)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(MungPlex PRIVATE ${LOVENSE_LIB_DIR}/Release/LovenseToys.lib)

    add_custom_command(TARGET MungPlex POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LOVENSE_LIB_DIR}/Release/LovenseToys.dll
        ${OUT_DIR}/x64-Release/MungPlex)
endif()

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

if (WIN32)
    set(APP_MANIFEST "${CMAKE_CURRENT_SOURCE_DIR}/MungPlex.manifest")
    set_target_properties(
        MungPlex PROPERTIES 
        LINK_FLAGS "/MANIFEST /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
    )
endif()