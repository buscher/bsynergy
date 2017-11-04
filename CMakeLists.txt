cmake_minimum_required(VERSION 3.5)
project(bsynergy)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5 COMPONENTS Core Widgets Gui Network REQUIRED)

# assume default debug for now
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif()

if(UNIX)
    find_package(X11 REQUIRED)

    if(X11_XTest_FOUND)
        message("X11 XTest found.")
    else()
        message(FATAL_ERROR "X11 XTest not found.")
    endif()
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII -DQT_USE_FAST_OPERATOR_PLUS -DQT_USE_FAST_CONCATENATION")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DQT_STRICT_ITERATORS)
    SET(CMAKE_VERBOSE_MAKEFILE 1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wsign-compare -Wpointer-arith -Wmissing-declarations")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpacked -Wswitch-enum -Wstrict-aliasing=2 -Winit-self -Winline")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunsafe-loop-optimizations -Wdouble-promotion  -Wformat=2")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wzero-as-null-pointer-constant -Wshadow")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()


add_subdirectory(src)
