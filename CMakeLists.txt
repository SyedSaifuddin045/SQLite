cmake_minimum_required(VERSION 3.8)
project(sqlite)

# Enable export of compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG d144031940543e15423a25ae5a8a74141044862f
)

FetchContent_MakeAvailable(googletest)

# Set directories
set(include_dir "${PROJECT_SOURCE_DIR}/include/")
set(library_dir "${PROJECT_SOURCE_DIR}/lib/")
set(source_dir "${PROJECT_SOURCE_DIR}/src/")
set(test_dir "${PROJECT_SOURCE_DIR}/tests/")

# Collect source and header files
file(GLOB_RECURSE source_files "${source_dir}/*.cpp" "${source_dir}/*.c")
file(GLOB_RECURSE header_files "${include_dir}/*.h")
file(GLOB_RECURSE test_files "${test_dir}/*.cpp")

# Include directories
include_directories(${include_dir})
include_directories(${library_dir})

# Platform-specific settings
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32_LEAN_AND_MEAN")
    file(GLOB LIB_FILES "${library_dir}/*.lib" "${library_dir}/*.dll")
elseif (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPLE")
    file(GLOB LIB_FILES "${library_dir}/*.dylib")
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIX")
    file(GLOB LIB_FILES "${library_dir}/*.so")
endif()

enable_testing()

# Create the main executable target
add_executable(${PROJECT_NAME} ${source_files} ${header_files})
target_link_libraries(${PROJECT_NAME} ${LIB_FILES})

# Create the test target
add_executable(DatabaseTest ${test_files})
target_link_libraries(DatabaseTest ${LIB_FILES} GTest::gtest_main)
target_compile_definitions(DatabaseTest PRIVATE EXECUTABLE_NAME=\"${PROJECT_NAME}\")
add_custom_command(
    TARGET DatabaseTest POST_BUILD
    COMMAND ./DatabaseTest
    COMMENT "Running tests after build"
)
# Discover and register the tests
include(GoogleTest)
gtest_discover_tests(DatabaseTest
    PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Print configuration summary
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Include Directory: ${include_dir}")
message(STATUS "Library Directory: ${library_dir}")
message(STATUS "Source Directory: ${source_dir}")
message(STATUS "Test Directory: ${test_dir}")
