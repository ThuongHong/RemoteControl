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
    src/App.cpp
    src/App.h
    src/RemoteControlDraft.cpp
    src/RemoteControlDraft.h
)

# Create an executable target
add_executable(client ${SOURCE_FILES})

# Set the path for your assets folder
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

# Set the output directory (where the binary is built)
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/assets")

# Copy image assets to the build directory
add_custom_command(
    TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ASSETS_DIR} ${OUTPUT_DIR}
)

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
set(CURL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/out/build/curl/include")  # Update this path
set(CURL_LIBRARY "${CMAKE_SOURCE_DIR}/out/build/curl/build/lib/Release/libcurl.lib")  # Update this path
find_package(CURL REQUIRED)
# target_link_libraries(client PRIVATE CURL::libcurl)
target_link_libraries(client PRIVATE ${CURL_LIBRARY})
target_compile_definitions(client PRIVATE CURL_STATICLIB)
target_link_libraries(client PRIVATE ws2_32 wldap32 crypt32)


# Find and link wxWidgets libraries
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})
target_link_libraries(client PRIVATE ${wxWidgets_LIBRARIES})

# Include directories for CURL and wxWidgets
target_include_directories(client PRIVATE ${CURL_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS})
