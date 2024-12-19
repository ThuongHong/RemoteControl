cmake_minimum_required(VERSION 3.24)

# Project settings
project(server VERSION 1.0.0 LANGUAGES CXX)

# Build configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Source files
set(SOURCE_FILES
    src/Server.cpp
    src/ServerHandler.cpp
    src/ServerHandler.h
)

# Define executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# OpenCV configuration
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${OpenCV_INCLUDE_DIRS}
)

# Windows specific configuration
if(WIN32)
    # GDI+ configuration
    find_library(GDI_PLUS_LIB Gdiplus)
    if(NOT GDI_PLUS_LIB)
        message(WARNING "GDI+ library not found")
    endif()

    # Windows definitions
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    
    # Link Windows libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}
        ${GDI_PLUS_LIB}
        ws2_32
        gdi32
        user32
        kernel32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
    )
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

if(OpenCV_FOUND)
    install(FILES 
        "${OpenCV_DIR}/../bin/opencv_world4100.dll"
        DESTINATION bin
    )
endif()

# CPack Configuration
set(CPACK_PACKAGE_NAME "Remote Control via Gmail - Server")
set(CPACK_PACKAGE_VENDOR "HCMUS-23TNT1")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Remote Control via Gmail - Server Application")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Remote Control via Gmail - Server")

# NSIS specific settings
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_PACKAGE_NAME "Remote Control via Gmail - Server")
set(CPACK_NSIS_DISPLAY_NAME "Remote Control via Gmail - Server")
set(CPACK_NSIS_CONTACT "luuthuonghong@gmail.com")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)

# Start Menu shortcuts
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Remote Control via Gmail - Server.lnk' '$INSTDIR\\\\bin\\\\server.exe'"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Remote Control via Gmail - Server.lnk'"
)

# Desktop shortcut
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
    "CreateShortCut '$DESKTOP\\\\Remote Control via Gmail - Server.lnk' '$INSTDIR\\\\bin\\\\server.exe'"
)
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
    "Delete '$DESKTOP\\\\Remote Control via Gmail - Server.lnk'"
)

include(CPack)