# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

project ("MungPlex")
set(CMAKE_CXX_STANDARD 23)

add_subdirectory ("MungPlex")
#add_subdirectory ("include/Xertz")#maybe can be removed
#add_subdirectory ("include/Xertz/OperativeArray")#maybe can be removed
#add_subdirectory ("include/Xertz/LitColor")#maybe can be removed
#add_subdirectory ("include/Xertz/MorphText")#maybe can be removed

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
