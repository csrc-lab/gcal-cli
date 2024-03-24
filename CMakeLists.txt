cmake_minimum_required(VERSION 3.20)

# Set the project name and version
project(gcal-cli VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(DEFAULT_BUILD_TYPE "Debug")

# Include directories
include_directories(include)

# Add the executable
add_executable(gcal-cli src/main.cpp src/GoogleOauth.cpp src/ConfigManager.cpp src/TokenManager.cpp src/GoogleEventsAPI.cpp)

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()


include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 871ed52d350214a034f6ef8a3b8f51c5ce1bd400) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

target_link_libraries(gcal-cli PRIVATE cpr::cpr)
