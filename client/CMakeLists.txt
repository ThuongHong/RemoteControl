cmake_minimum_required(VERSION 3.24)

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Project name
project(client)

# Source files for the executable
set(SOURCE_FILES
    src/client_socket.cpp
    src/client_socket.h
    src/gmail.h
    src/gmail.cpp
    src/client.h
    src/client.cpp
    src/main.cpp
)

# Create an executable target
add_executable(client ${SOURCE_FILES})

# Specify C++ standard
set_property(TARGET client PROPERTY CXX_STANDARD 20)

# Display build type information
if(CMAKE_BUILD_TYPE)
    message(STATUS "${CMAKE_BUILD_TYPE} build")
else()
    message(STATUS "No specific build type selected, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release) # Default to Release if not specified
endif()

# Find and link CURL library
find_package(CURL REQUIRED)
target_link_libraries(client PRIVATE CURL::libcurl)

# Find and link wxWidgets libraries
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})
target_link_libraries(client PRIVATE ${wxWidgets_LIBRARIES})

# Include directories for CURL and wxWidgets
target_include_directories(client PRIVATE ${CURL_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS})
