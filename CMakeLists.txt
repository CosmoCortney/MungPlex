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
    "include/Xertz/OperativeArray/OperativeArray"
    "include/Xertz/LitColor/LitColor"
    "include/Xertz/MorphText/MorphText"
    "include/ImGuiFileDialog"
    "include/ImGui_MEmoryEditor/imgui_memory_editor"
)
