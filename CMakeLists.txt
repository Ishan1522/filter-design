cmake_minimum_required(VERSION 3.14)

# Set project name and language
project(filter-design 
    VERSION 1.0
    DESCRIPTION "Filter Design Application"
    LANGUAGES CXX
)

# Include WPILib
include(FetchContent)
FetchContent_Declare(
    wpilib
    GIT_REPOSITORY https://github.com/wpilibsuite/allwpilib.git
    GIT_TAG v2025.3.2
)
FetchContent_MakeAvailable(wpilib)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source files
set(SOURCES
    source/main.cpp
    source/Butterworth.cpp
    source/WpilogReader.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        wpilibc::wpilibc
        wpilibc::wpiutil
        wpilibc::glass
        wpilibc::glassnt
        wpilibc::imgui
        wpilibc::imgui_impl_glfw
        wpilibc::imgui_impl_opengl3
        OpenGL::GL
        glfw
)

# Enable position independent code
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
