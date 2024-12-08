cmake_minimum_required(VERSION 3.24)

project(server)

# Add source to this project's executable.
set(SOURCE_FILES
    # src/server_task.cpp
    # src/server_task.h
    # src/server_socket.cpp
    # src/server_socket.h
    src/server.cpp
    src/server_handler.cpp
    src/server_handler.h
)

# Add an executable target first
add_executable(server ${SOURCE_FILES})

# Specify the C++ standard
set_property(TARGET server PROPERTY CXX_STANDARD 20)

# Find and link OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    target_include_directories(server PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(server PRIVATE ${OpenCV_LIBS})
else()
    message(FATAL_ERROR "OpenCV not found")
endif()

# Find and link GDI+
find_library(GDI_PLUS_LIB Gdiplus)
if(GDI_PLUS_LIB)
    target_link_libraries(server PRIVATE ${GDI_PLUS_LIB})
else()
    message(WARNING "GDI+ library not found")
endif()

# Add Windows-specific libraries
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    target_link_libraries(server PRIVATE
        ws2_32
        gdi32
        user32
        advapi32
    )
endif()
