cmake_minimum_required(VERSION 3.25)

project(BST)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT WIN32)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[m")
    set(Green "${Esc}[32m")
    set(Orange "${Esc}[33m")
endif()

message(STATUS "Configuring project ${PROJECT_NAME}")

add_subdirectory(src)
add_subdirectory(tests)
