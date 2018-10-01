# ┌──────────────────────────────────────────────────────────────────┐
# │  Projects Settings                                               │
# └──────────────────────────────────────────────────────────────────┘
cmake_minimum_required (VERSION 3.2)
project (ScanBody LANGUAGES C CXX)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Compilation flags                                               │
# └──────────────────────────────────────────────────────────────────┘
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_VERBOSE_MAKEFILE 1) # 1 should be used for debugging
set (CMAKE_SUPPRESS_REGENERATION TRUE) # Suppresses ZERO_CHECK
if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()


# ┌──────────────────────────────────────────────────────────────────┐
# │  Load libraries                                                  │
# └──────────────────────────────────────────────────────────────────┘
# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE STRING "" FORCE)
set(GLFW_INSTALL        OFF CACHE STRING "" FORCE)
add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

# glew
add_definitions(-DGLEW_STATIC)
option(glew-cmake_BUILD_SHARED "" OFF)
add_subdirectory(lib/glew)
include_directories(lib/glew/include lib/glew/src)

# glm
include_directories(lib/glm)

# imgui
# add_subdirectory(lib/imgui/examples)
include_directories(lib/imgui lib/imgui/examples lib/imgui/misc)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Build executable                                                │
# └──────────────────────────────────────────────────────────────────┘
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)
file(GLOB_RECURSE source_files src/*)
list(APPEND source_files
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_widgets.cpp)
list(APPEND source_files
    lib/imgui/examples/imgui_impl_opengl3.cpp
    lib/imgui/examples/imgui_impl_glfw.cpp)


add_executable(${PROJECT_NAME}  ${source_files})
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} libglew_static)