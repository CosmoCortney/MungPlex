# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

project ("MungPlex")
set(CMAKE_CXX_STANDARD 23)

add_subdirectory ("MungPlex")

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
)

# Enable Hot Reload for MSVC compilers if supported
# if (POLICY CMP0141)
#   cmake_policy(SET CMP0141 NEW)
#   set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
# endif()