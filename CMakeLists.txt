# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Define your project name and the programming language(s) you will use.
project(HelloWorld CXX)

# Add an executable target named "hello" with the source file main.cpp
add_executable(hello main.cpp)
