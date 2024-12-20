cmake_minimum_required(VERSION 3.24)

# Project settings
project(client VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Source files
set(SOURCE_FILES
    src/main.cpp
    src/Gmail.h
    src/Gmail.cpp
    src/Client.h
    src/Client.cpp
    src/Base64.h
    src/Base64.cpp
    src/HTTPListener.h
    src/HTTPListener.cpp
    src/App.cpp
    src/App.h
    src/RemoteControlDraft.cpp
    src/RemoteControlDraft.h
    src/Panels.h
    src/Panels.cpp
)

# Define executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Dependencies paths
set(DEPS_ROOT "${CMAKE_SOURCE_DIR}/lib")
set(CURL_ROOT "${DEPS_ROOT}/curl")
set(OPENSSL_ROOT "${DEPS_ROOT}/OpenSSL-Win64")
set(WX_ROOT "${DEPS_ROOT}/WxWidget")

# Configure cURL
set(CURL_INCLUDE_DIR "${CURL_ROOT}/install/include")
set(CURL_LIBRARY_DIR "${CURL_ROOT}/install/lib")
add_definitions(-DCURL_STATICLIB)

# Configure OpenSSL
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT}/include")
set(OPENSSL_LIB_DIR "${OPENSSL_ROOT}/lib/VC/x64/MD")

# Configure wxWidgets
set(wxWidgets_ROOT_DIR ${WX_ROOT})
set(wxWidgets_LIB_DIR "${WX_ROOT}/lib/vc_x64_lib")
set(wxWidgets_INCLUDE_DIRS "${WX_ROOT}/include")
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CURL_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    # Third party libraries
    "${CURL_LIBRARY_DIR}/libcurl.lib"
    "${OPENSSL_LIB_DIR}/libssl.lib"
    "${OPENSSL_LIB_DIR}/libcrypto.lib"
    ${wxWidgets_LIBRARIES}
    
    # Windows system libraries
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

# Asset paths
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/assets")

# Copy assets to build directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Copying from ${ASSETS_DIR} to ${ASSETS_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "${ASSETS_OUTPUT_DIR}"
    COMMENT "Copying assets to build directory"
)

# Make client depend on assets
add_dependencies(${PROJECT_NAME} copy_assets)

# Installation rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install assets to bin directory
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    DESTINATION bin
    FILES_MATCHING 
    PATTERN "*.png"
    PATTERN "*.ico"
)

# CPack Configuration
set(CPACK_PACKAGE_NAME "Remote Control via Gmail - Client")
set(CPACK_PACKAGE_VENDOR "HCMUS-23TNT1")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Remote Control via Gmail - Client Application")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Remote Control via Gmail - Client")

# NSIS specific settings
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_PACKAGE_NAME "Remote Control via Gmail - Client")
set(CPACK_NSIS_DISPLAY_NAME "Remote Control via Gmail - Client") 
set(CPACK_NSIS_CONTACT "luuthuonghong@gmail.com")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)

# Start Menu shortcuts with assets path
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Remote Control via Gmail - Client.lnk' '$INSTDIR\\\\bin\\\\client.exe' '' '$INSTDIR\\\\bin\\\\assets\\\\icon.ico'"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Remote Control via Gmail - Client.lnk'"
)

# Desktop shortcut with assets path
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
    "CreateShortCut '$DESKTOP\\\\Remote Control via Gmail - Client.lnk' '$INSTDIR\\\\bin\\\\client.exe' '' '$INSTDIR\\\\bin\\\\assets\\\\icon.ico'"
)
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
    "Delete '$DESKTOP\\\\Remote Control via Gmail - Client.lnk'"
)

include(CPack)