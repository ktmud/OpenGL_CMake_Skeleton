cmake_minimum_required (VERSION 3.2)
project (RealSenseScanner LANGUAGES C CXX)


#---------- Compilation flags -------------------------

# Enable modern C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_VERBOSE_MAKEFILE 1) # 1 should be used for debugging
set (CMAKE_SUPPRESS_REGENERATION TRUE) # Suppresses ZERO_CHECK
set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # Save the command line compile commands in the build output

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

#---------- Dependencies -------------------------
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

include_directories(lib/glew/include)

# glm
include_directories(lib/glm)

# imgui
# add_subdirectory(lib/imgui/examples)
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)
include_directories(lib/imgui lib/imgui/examples lib/imgui/misc)

# librealsense
if(WIN32)
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(RS_LIB_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x64")
        set(RS_DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x64")
    else()
        set(RS_LIB_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x86")
        set(RS_DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x86")
    endif()
    set(ADDITIONAL_INCLUDE_DIRS "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\include")
endif()
find_library(REALSENSE2_FOUND realsense2 HINTS ${RS_LIB_DIR} REQUIRED)
if(NOT REALSENSE2_FOUND)
    SET(REALSENSE2_FOUND "realsense2")
    message(WARN "Failed to find_library(realsense2)")
endif()
message("${ADDITIONAL_INCLUDE_DIRS}")
include_directories(${ADDITIONAL_INCLUDE_DIRS})


# ----- Include header files as source files -----------------
file(GLOB_RECURSE source_files src/*)
list(APPEND source_files
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_widgets.cpp)
list(APPEND source_files
    lib/imgui/examples/imgui_impl_opengl3.cpp
    lib/imgui/examples/imgui_impl_glfw.cpp)


# ------- Build Target -------------
add_executable(${PROJECT_NAME}  ${source_files})
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} libglew_static ${REALSENSE2_FOUND})

# Copy assets (fonts, etc) for GUI
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/assets/
                       $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

# Post Build script to copy realsense2.dll
if(WIN32)
    message(STATUS "Adding Post build script to copy realsense2.dll to project's binary folder")
    message(STATUS "Will try to copy from `${RS_DLL_DIR}` to `${PROJECT_BINARY_DIR}`")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${RS_DLL_DIR}/realsense2.dll"
            ${PROJECT_BINARY_DIR})
endif()

install(
    TARGETS
    ${PROJECT_NAME}
    RUNTIME DESTINATION
    ${CMAKE_INSTALL_PREFIX}/bin
)