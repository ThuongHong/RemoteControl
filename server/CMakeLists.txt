# CMakeLists.txt : CMake project for server, include source and define
# project specific logic here.

cmake_minimum_required(VERSION 3.24)

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(server)

# Add source to this project's executable.
set(SOURCE_FILES
    src/server_task.cpp
    src/server_task.h
    src/server_socket.cpp
    src/server_socket.h
    src/server.cpp
)

# Add an executable target
add_executable(server ${SOURCE_FILES})

# Specify the C++ standard
set_property(TARGET server PROPERTY CXX_STANDARD 20)

# Optional: Define compilation flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build")
else()
    message(STATUS "No specific build type selected, defaulting to Release")
endif()

# Find and link GDI+
find_library(GDI_PLUS_LIB Gdiplus)
if(GDI_PLUS_LIB)
    target_link_libraries(server PRIVATE ${GDI_PLUS_LIB})
else()
    message(WARNING "GDI+ library not found. Make sure it's installed.")
endif()

# Find and link OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    target_link_libraries(server PRIVATE ${OpenCV_LIBS})
    include_directories(${OpenCV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "OpenCV not found. Please install OpenCV.")
endif()

# TODO: Add tests and install targets if needed.
