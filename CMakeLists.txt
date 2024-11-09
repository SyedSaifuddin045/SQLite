cmake_minimum_required(VERSION 3.8)
project(sqlite)

# Enable export of compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Set directories
set(include_dir "${PROJECT_SOURCE_DIR}/include/")
set(library_dir "${PROJECT_SOURCE_DIR}/lib/")
set(source_dir "${PROJECT_SOURCE_DIR}/src/")

# Collect source and header files
file(GLOB_RECURSE source_files "${source_dir}/*.cpp" "${source_dir}/*.c")
file(GLOB_RECURSE header_files "${include_dir}/*.h")

# Include directories
include_directories(${include_dir})
include_directories(${library_dir})

# Platform-specific settings
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32_LEAN_AND_MEAN")
    # Collect .lib and .dll files for Windows
    file(GLOB LIB_FILES "${library_dir}/*.lib" "${library_dir}/*.dll")
elseif (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPLE")
    # Collect .dylib files for macOS
    file(GLOB LIB_FILES "${library_dir}/*.dylib")
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIX")
    # Collect .so files for Linux/Unix
    file(GLOB LIB_FILES "${library_dir}/*.so")
endif()

# Create the executable target
add_executable(${PROJECT_NAME} ${source_files} ${header_files})

# Link libraries to the target
target_link_libraries(${PROJECT_NAME} ${LIB_FILES})

# Print configuration summary
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Include Directory: ${include_dir}")
message(STATUS "Library Directory: ${library_dir}")
message(STATUS "Source Directory: ${source_dir}")

