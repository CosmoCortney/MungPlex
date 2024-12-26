# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

project ("MungPlex")
set(CMAKE_CXX_STANDARD 23)
add_subdirectory ("MungPlex")

#Download and install Lovense SDK
set(LOVENSE_SDK_VERSION "3.1.4" CACHE STRING "Lovense SDK Version")
set(LOVENSE_SDK_FOLDER "${CMAKE_SOURCE_DIR}/MungPlex/source/DeviceControl/lovense-win-sdk-${LOVENSE_SDK_VERSION}" CACHE STRING "Lovense SDK Path")
#message(STATUS "Lovense SDK Folder: ${LOVENSE_SDK_FOLDER}")
set(LOVENSE_SDK_URL "https://github.com/lovense/Lovense-Windows-SDK/releases/download/v${LOVENSE_SDK_VERSION}/lovense-win-sdk-${LOVENSE_SDK_VERSION}.zip")
#message(STATUS "Lovense SDK URL: ${LOVENSE_SDK_URL}")
set(LOVENSE_SDK_ZIP_PATH "${LOVENSE_SDK_FOLDER}/lovense-win-sdk-${LOVENSE_SDK_VERSION}.zip")
#message(STATUS "Lovense SDK ZIP File Path: ${LOVENSE_SDK_PATH}")

if(NOT EXISTS "${LOVENSE_SDK_FOLDER}")
    message(STATUS "Downloading Lovense SDK from: ${LOVENSE_SDK_URL}")

    file(DOWNLOAD "${LOVENSE_SDK_URL}"
        "${LOVENSE_SDK_ZIP_PATH}"
        SHOW_PROGRESS
        STATUS download_status
        LOG download_log
    )

    # Check if download was successful
    list(GET download_status 0 download_status_code)
    if(download_status_code EQUAL 0)
        message(STATUS "Download successful.")
    else()
        message(FATAL_ERROR "Failed to download the file. Status: ${download_status}")
    endif()

    # Extract the zip file
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xfz "${LOVENSE_SDK_ZIP_PATH}"
        WORKING_DIRECTORY "${LOVENSE_SDK_FOLDER}"
        RESULT_VARIABLE result
    )

    if(result EQUAL 0)
        message(STATUS "Extraction successful.")
        message(STATUS "Deleting SDK ZIP file.")
        file(REMOVE "${LOVENSE_SDK_ZIP_PATH}")
    else()
        message(FATAL_ERROR "Failed to extract the file.")
    endif()

else()
    message(STATUS "Lovense SDK already exists. No need to download.")
endif()

#download and install FTDI lib
set(FT_LIB_VERSION "2.12.36.4" CACHE STRING "FT Lib Version")
set(FT_LIB_FOLDER "${CMAKE_SOURCE_DIR}/MungPlex/source/ftdi/CDM-v${FT_LIB_VERSION}-WHQL-Certified" CACHE STRING "FT Lib Path")
#message(STATUS "FT Lib Folder: ${FT_LIB_FOLDER}")
set(FT_LIB_URL "https://ftdichip.com/wp-content/uploads/2023/09/CDM-v${FT_LIB_VERSION}-WHQL-Certified.zip")
#message(STATUS "FT Lib URL: ${FT_LIB_URL}")
set(FT_LIB_ZIP_PATH "${FT_LIB_FOLDER}/CDM-v${FT_LIB_VERSION}-WHQL-Certified.zip")
#message(STATUS "FT Lib ZIP File Path: ${FT_LIB_ZIP_PATH}")

if(NOT EXISTS "${FT_LIB_FOLDER}")
    message(STATUS "Downloading FTDI Lib from: ${FT_LIB_URL}")

    file(DOWNLOAD "${FT_LIB_URL}"
        "${FT_LIB_ZIP_PATH}"
        SHOW_PROGRESS
        STATUS download_status
        LOG download_log
    )

    # Check if download was successful
    list(GET download_status 0 download_status_code)
    if(download_status_code EQUAL 0)
        message(STATUS "Download successful.")
    else()
        message(FATAL_ERROR "Failed to download the file. Status: ${download_status}")
    endif()

    # Extract the zip file
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xfz "${FT_LIB_ZIP_PATH}"
        WORKING_DIRECTORY "${FT_LIB_FOLDER}"
        RESULT_VARIABLE result
    )

    if(result EQUAL 0)
        message(STATUS "Extraction successful.")
        message(STATUS "Deleting Lib ZIP file.")
        file(REMOVE "${FT_LIB_ZIP_PATH}")
    else()
        message(FATAL_ERROR "Failed to extract the file.")
    endif()

else()
    message(STATUS "FTDI Lib already exists. No need to download.")
endif()


target_include_directories(MungPlex PRIVATE
    "include/imgui"
    "include/glad"
    "include/Xertz"
    "include/MemoryCompare"
    "include/MemoryCompare/include/OperativeArray/OperativeArray"
    "include/MemoryCompare/include/LitColor/LitColor"
    "include/MemoryCompare/include/MorphText/MorphText"
    "include/ImGuiFileDialog"
    "include/ImGui_MemoryEditor/imgui_memory_editor"
    "MungPlex/source/ftdi/CDM-v${FT_LIB_VERSION}-WHQL-Certified"
    "MungPlex/source/DeviceControl"
    "MungPlex/source/Cheats"
    "MungPlex/source/CheatSearch"
    "MungPlex/source/Connection"
    "MungPlex/source/Connection/ConsoleConnectionWrappers"
    "MungPlex/source/ContextMenu"
    "MungPlex/source/DataConversion"
    "MungPlex/source/DeviceControl/lovense-win-sdk-${LOVENSE_SDK_VERSION}/WinSDK_Release/includes"
    "MungPlex/source/Helpers"
    "MungPlex/source/Helpers/WidgetHelpers"
    "MungPlex/source/MemoryViewer"
    "MungPlex/source/PointerSearch"
    "MungPlex/source/ProcessInformation"
    "MungPlex/source/ProcessInformation/Emulators"
    "MungPlex/source/Settings"
    "MungPlex/source/WatchControl"
)

# Enable Hot Reload for MSVC compilers if supported
# if (POLICY CMP0141)
#   cmake_policy(SET CMP0141 NEW)
#   set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
# endif()