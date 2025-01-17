cmake_minimum_required(VERSION 3.16)

# Set the project name
project(MyProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(MyProject src/main.cpp)

# Include directories
target_include_directories(MyProject PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include")
