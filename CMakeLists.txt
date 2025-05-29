cmake_minimum_required(VERSION 3.15)

# Set vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Set project name and language
project(filter_design)

# Set build directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

# Add imgui as a subdirectory
add_subdirectory(imgui)

# Configure imnodes
set(IMNODES_STANDALONE_PROJECT OFF)
set(IMNODES_IMGUI_TARGET_NAME imgui)
add_subdirectory(imnodes)

# Add ImGui backend implementation files
target_sources(imgui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source files
set(SOURCES
    source/main.cpp
    source/ui/FilterDesignUI.cpp
    source/filter/LowPassFilter.cpp
    source/pipeline/FilterPipeline.cpp
)

# Add header files
set(HEADERS
    include/filter/Filter.h
    include/filter/LowPassFilter.h
    include/ui/FilterDesignUI.h
    include/pipeline/FilterPipeline.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/imnodes
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${OPENGL_INCLUDE_DIR}
        ${vcpkg_installed_DIR}/x64-windows/include
)

# Add include directories for imgui
target_include_directories(imgui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
        ${GLFW_INCLUDE_DIRS}
        ${vcpkg_installed_DIR}/x64-windows/include
)

# Add include directories for imnodes
target_include_directories(imnodes
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/imnodes
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    imnodes
    ${OPENGL_LIBRARIES}
    glfw
    glad::glad
)

target_link_libraries(imgui
    PUBLIC
        glfw
)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add Windows-specific definitions
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        GLFW_INCLUDE_NONE
    )
endif()
