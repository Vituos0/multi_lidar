cmake_minimum_required(VERSION 3.20)

project(EdgeLidar LANGUAGES CXX)

# -----------------------------
# C++ standard
# -----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------
# Source files
# -----------------------------
set(SOURCES
    main.cpp

    lidar/PavoParser.cpp
    lidar/UdpReceiver.cpp
)

# -----------------------------
# Executable
# -----------------------------
add_executable(EdgeLidar ${SOURCES})

# -----------------------------
# Include directories
# -----------------------------
target_include_directories(EdgeLidar PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lidar
    ${CMAKE_CURRENT_SOURCE_DIR}/lidar/common
)

# -----------------------------
# Windows networking
# -----------------------------
if (WIN32)
    target_link_libraries(EdgeLidar PRIVATE ws2_32)
endif()
