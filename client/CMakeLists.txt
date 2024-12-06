cmake_minimum_required(VERSION 3.24)

# Project name
project(client)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

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
    src/App.cpp
    src/App.h
    src/RemoteControlDraft.cpp
    src/RemoteControlDraft.h
)

# Create executable
add_executable(client ${SOURCE_FILES})

# Copy assets to build directory
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/assets")
add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${OUTPUT_DIR})

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No specific build type selected, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release)
else()
    message(STATUS "${CMAKE_BUILD_TYPE} build")
endif()

# Set cURL and OpenSSL paths (replace with your paths)
set(CURL_DIR "${CMAKE_SOURCE_DIR}/lib/curl")
set(OPENSSL_DIR "${CMAKE_SOURCE_DIR}/lib/OpenSSL-Win64")

# Include directories for cURL and OpenSSL
set(CURL_INCLUDE_DIR "${CURL_DIR}/install/include")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_DIR}/include")
target_include_directories(client PRIVATE ${CURL_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

# Link cURL and OpenSSL libraries
set(CURL_LIBRARY_DIR "${CURL_DIR}/install/lib")
set(OPENSSL_LIB_DIR "${OPENSSL_DIR}/lib/VC/x64/MD")
target_link_libraries(client PRIVATE
    "${CURL_LIBRARY_DIR}/libcurl.lib"
    "${OPENSSL_LIB_DIR}/libssl.lib"
    "${OPENSSL_LIB_DIR}/libcrypto.lib"
)

# Define static library for cURL
add_definitions(-DCURL_STATICLIB)

# Find and link wxWidgets
set(wxWidgets_ROOT_DIR "C:/Users/PC/Documents/wxWidget")
set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/vc_x64_lib")
set(wxWidgets_INCLUDE_DIRS "${wxWidgets_ROOT_DIR}/include")
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})
target_link_libraries(client PRIVATE ${wxWidgets_LIBRARIES})

# Link Windows libraries
target_link_libraries(client PRIVATE
    Crypt32.lib
    Wldap32.lib
    ws2_32.lib
    gdi32.lib
    kernel32.lib
    user32.lib
    winspool.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    comdlg32.lib
    advapi32.lib
)
