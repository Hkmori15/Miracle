cmake_minimum_required(VERSION 3.30)
project(miracle C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)

# Find packages
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)

# Use pkg-config to find libraries
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(CGLM REQUIRED cglm)
pkg_check_modules(ASSIMP REQUIRED assimp)
pkg_check_modules(BULLET REQUIRED bullet)
pkg_check_modules(OPENAL REQUIRED openal)
pkg_check_modules(SNDFILE REQUIRED sndfile)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/stb)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add include directories from pkg-config
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${CGLM_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})
include_directories(${BULLET_INCLUDE_DIRS})
include_directories(${OPENAL_INCLUDE_DIRS})
include_directories(${SNDFILE_INCLUDE_DIRS})

# Source files
file(GLOB_RECURSE C_SOURCES "src/*.c")
file(GLOB_RECURSE CXX_SOURCES "src/*.cpp")
set(SOURCES ${C_SOURCES} ${CXX_SOURCES})

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${CGLM_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    ${BULLET_LIBRARIES}
    ${OPENAL_LIBRARIES}
    ${SNDFILE_LIBRARIES}
    m
    dl
)

# Add library directories
target_link_directories(${PROJECT_NAME} PRIVATE
    ${GLFW_LIBRARY_DIRS}
    ${CGLM_LIBRARY_DIRS}
    ${ASSIMP_LIBRARY_DIRS}
    ${BULLET_LIBRARY_DIRS}
    ${OPENAL_LIBRARY_DIRS}
    ${SNDFILE_LIBRARY_DIRS}
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra
    ${GLFW_CFLAGS_OTHER}
    ${CGLM_CFLAGS_OTHER}
    ${ASSIMP_CFLAGS_OTHER}
    ${BULLET_CFLAGS_OTHER}
    ${OPENAL_CFLAGS_OTHER}
    ${SNDFILE_CFLAGS_OTHER}
)

# Set C standard for C files only
set_source_files_properties(${C_SOURCES} PROPERTIES COMPILE_FLAGS "-std=c2x")

# Create assets directory in build
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets to build directory"
)