# CMakeLists.txt : CMake project for client, include source and define
# project specific logic here.

cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(client)

# Add source to this project's executable.
set(SOURCE_FILES
    "src/client_socket.cpp"
    "src/client_socket.h" 
    "src/gmail.cpp"
    "src/gmail.h"
    "src/client.cpp"
)

# Add an executable target
add_executable(client ${SOURCE_FILES})

# Specify the C++ standard after the target is created
set_property(TARGET client PROPERTY CXX_STANDARD 20)

# Optional: Define compilation flags based on build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build")
else()
    message(STATUS "No specific build type selected, defaulting to Release")
endif()

# TODO: Add tests and install targets if needed.

find_package(CURL REQUIRED)
target_link_libraries(client CURL::libcurl)

include_directories(${client})