# CMakeList.txt : CMake project for MungPlex, include source and define
# project specific logic here.

# Add source to this project's executable.
add_executable (MungPlex    "main.cpp" 
                            "Connection.cpp"
                            "../include/imgui/imgui.cpp"
                            "../include/imgui/imgui_demo.cpp"
                            "../include/imgui/imgui_draw.cpp"
                            "../include/imgui/imgui_tables.cpp"
                            "../include/imgui/imgui_widgets.cpp"
                            "../include/imgui/backends/imgui_impl_opengl3.cpp"
                            "../include/imgui/backends/imgui_impl_glfw.cpp"
                            "../include/Xertz/MemDump.cpp"
                            "../include/Xertz/MemoryRegion.cpp"
                            "../include/Xertz/ProcessInfo.cpp"
                            "../include/Xertz/SystemInfo.cpp"
                            "Connection.h"
                            "Connection.cpp"
                            "ProcessInformation.h"
                            "ProcessInformation.cpp"
                            "Search.cpp"
                            "HelperFunctions.h"
               )


include_directories(imgui)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MungPlex PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
install (TARGETS MungPlex DESTINATION "MungPlex/bin")

project(MungPlex VERSION 2.0)
configure_file(MungPlexConfig.h.in MungPlexConfig.h) 

target_link_libraries(MungPlex glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(MungPlex OpenGL::GL)


target_include_directories(MungPlex PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/../include/imgui"
                           "${PROJECT_SOURCE_DIR}/../include/glfw"
                           "${PROJECT_SOURCE_DIR}/../include/glad/include"
                           "${PROJECT_SOURCE_DIR}/../include/glad/src"
                           "${PROJECT_SOURCE_DIR}/../include/meojson/include"
                           "${PROJECT_SOURCE_DIR}/../include/Xertz"
                           )


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)