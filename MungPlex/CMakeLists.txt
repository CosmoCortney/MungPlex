# CMakeList.txt : CMake project for MungPlex, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.16)
project(MungPlex VERSION 2.6.0)

find_package(Lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread filesystem iostreams)
#find_package(libftdi CONFIG REQUIRED)
##find_package(libftdi1 CONFIG REQUIRED)
##find_library(FTDI1_LIB libftdi1)
#find_library(USB_LIB libusb)
##find_package(libusb)
#find_package(PkgConfig)
#pkg_check_modules(libusb-1.0 REQUIRED libusb-1.0)
find_library(CPP_GAME_SDK discord_game_sdk_cpp)
find_library(DISCORD_GAME_SDK_LIB discord_game_sdk)
find_library(DISCORD_GAME_SDK_CPP_LIB discord_game_sdk_cpp)

#message(STATUS "VCPKG_ROOT is set to: $ENV{VCPKG_ROOT}")

#if(DEFINED ENV{VCPKG_ROOT})
#    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
#else()
#    message(FATAL_ERROR "Please set the VCPKG_ROOT environment variable.")
#endif()


#set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

#find_library(FTDI1_LIB NAMES ftdi1 PATHS "${VCPKG_ROOT}/installed/x64-windows/lib/")

# Generate MungPlexConfig.hpp using MungPlexConfig.hpp.in
configure_file(MungPlexConfig.hpp.in ${CMAKE_BINARY_DIR}/MungPlexConfig.hpp @ONLY)

# Add the main executable target
add_executable (MungPlex
    CheatConvert.hpp
    CheatConvert.cpp
    Cheats.hpp
    Cheats.cpp
    Connection.cpp
    Connection.hpp
    ContextMenuHelper.cpp
    ContextMenuHelper.hpp
    DataConversion.hpp
    DataConversion.cpp
    source/DeviceControl/DeviceControl.cpp
    source/DeviceControl/DeviceControl.hpp
    Discord.cpp
    Discord.hpp
    HelperFunctions.hpp
    source/DeviceControl/IDevice.cpp
    source/DeviceControl/IDevice.hpp
    source/DeviceControl/DeviceLovense_Impl.cpp
    source/DeviceControl/DeviceLovense_Impl.hpp
    Log.hpp
    Log.cpp
    source/DeviceControl/LovenseToy.cpp
    source/DeviceControl/LovenseToy.hpp
    main.cpp
    MemoryViewer.hpp
    MemoryViewer.cpp
    PointerSearch.hpp
    PointerSearch.cpp
    ProcessInformation.cpp
    ProcessInformation.hpp
    resources.rc
    Search.hpp
    Search.cpp
    Settings.hpp
    Settings.cpp
    USBGecko.hpp
    USBGecko.cpp
    WatchControl.cpp
    WatchControl.hpp
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
    #libftdi::ftdi
    #ftdi1
    #${libusb-1.0_LIBRARIES}
    ##usb-1.0
    ##${FTDI1_LIB}
    #${USB_LIB}
    ##${LIBFTDI1_LIBRARIES}
    ##${LIBUSB_LIBRARIES}
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