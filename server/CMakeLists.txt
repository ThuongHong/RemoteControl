cmake_minimum_required(VERSION 3.24)

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
