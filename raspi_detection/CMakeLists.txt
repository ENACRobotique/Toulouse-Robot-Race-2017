cmake_minimum_required(VERSION 2.8.3)
PROJECT(detect_line)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages

## System dependencies are found with CMake's conventions

FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE(FindPkgConfig)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -march=native -O2")

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
	src/
)

add_executable(detect_line
	src/detect.cpp 
)


target_link_libraries(detect_line 
	${OpenCV_LIBS}
#	yaml-cpp
)

add_definitions(-DDEBUG)

#############
## Install ##
#############

# TODO

#############
## Testing ##
#############

# TODO
