cmake_minimum_required(VERSION 3.16)
project(colorbox VERSION 0.0.1 LANGUAGES C)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON CACHE BOOL "Prevents cmake -S. -B.")
set(CMAKE_DISABLE_SOURCE_CHANGES ON CACHE BOOL "Prevent writing files to CMAKE_SOURCE_DIR under configure")

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0075 NEW)

set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_FIND_FRAMEWORK FIRST)

include(CMakeDependentOption)
include(GenerateExportHeader)
include(FetchContent)

# Enables debug output
cmake_dependent_option(COLORBOX_DEBUG_OUTPUT
  "Enable colorbox's internal debug output" ON 
  "CMAKE_BUILD_TYPE STREQUAL Debug" ON)

# Enables ASAN & Pedantic output
option(COLORBOX_DEBUG OFF)

if(APPLE)
  message(WARNING "OpenGL is deprecated on macOs 10.14 (Mojave)!")
endif()

# Kinda the whole basis of this program
find_package(OpenGL REQUIRED)

# Set the GLFW Flags
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_VULKAN_STATIC OFF)

# Add GLFW
add_subdirectory(${PROJECT_SOURCE_DIR}/dep/glfw EXCLUDE_FROM_ALL)

# Get all of our source files into a list
file(GLOB sources CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.c)
add_executable(${PROJECT_NAME})

# Add our source files & nanovg
target_sources(${PROJECT_NAME} PRIVATE ${sources})

# Include headers
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/dep/glad/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/dep>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INCLUDE_INSTALLDIR}>)

# Add all the preprocessor definitions for headers
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  $<$<BOOL:${COLORBOX_DEBUG}>:COLORBOX_DEBUG_OUTPUT>
  $<$<BOOL:${COLORBOX_DEBUG_OUTPUT}>:COLORBOX_DEBUG_OUTPUT>
    $<$<PLATFORM_ID:FreeBSD>:FreeBSD>
    $<$<PLATFORM_ID:OpenBSD>:OpenBSD>
    $<$<PLATFORM_ID:NetBSD>:NetBSD>
    $<$<PLATFORM_ID:Darwin>:OSX>)

# Set the C Standard to C99
target_compile_features(${PROJECT_NAME} PRIVATE c_std_99)

# Add warning output for the different compilers
target_compile_options(${PROJECT_NAME}
  PRIVATE
  $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
  $<$<C_COMPILER_ID:MSVC>:/W4>)

# Enables ASAN & Pedantics
if(COLORBOX_DEBUG)
  target_compile_options(${PROJECT_NAME}
    PUBLIC
    $<$<C_COMPILER_ID:MSVC>:/fsanitize=address>
    $<$<NOT:$<C_COMPILER_ID:MSVC>>: -g -fsanitize=address -fno-omit-frame-pointer -O2 -g>)
  
  target_link_options(${PROJECT_NAME}
    PUBLIC
    $<$<C_COMPILER_ID:MSVC>:/fsanitize=address>
    $<$<NOT:$<C_COMPILER_ID:MSVC>>:-g -fsanitize=address>)
endif()

# Link against the various libraries we need
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    OpenGL::GL
    $<$<NOT:$<PLATFORM_ID:Windows>>:m>
    glfw
    $<$<PLATFORM_ID:Windows>:XInput>)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_VISIBILITY_PRESET "hidden")

# Copy over resources into build folder
add_custom_target(${PROJECT_NAME}-resources ALL
  COMMAND
  ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/res"
  "${CMAKE_CURRENT_BINARY_DIR}/res"
  COMMENT "Copying resources directory"
  VERBATIM)
