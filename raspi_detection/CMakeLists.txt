cmake_minimum_required(VERSION 2.8.3)
PROJECT(detect_line)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages

## System dependencies are found with CMake's conventions

SET(raspicam_DIR /home/pi/raspicam-0.1.6/build)
SET(CMAKE_BUILD_TYPE Debug)
FIND_PACKAGE(OpenCV REQUIRED)
find_package(raspicam REQUIRED)

INCLUDE(FindPkgConfig)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -march=native -O2")
## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
	${raspicam_DIR}
	src/
)

IF  ( OpenCV_FOUND AND raspicam_CV_FOUND)
MESSAGE(STATUS "COMPILING OPENCV TESTS")

add_executable(detect_line
	src/detect.cpp 
)


#target_link_libraries(detect_line 
	#	${OpenCV_LIBS}
	#	#yaml-cpp
	#)


target_link_libraries (detect_line ${raspicam_CV_LIBS})

add_definitions(-DDEBUG)


ELSE()
MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()

#############
## Install ##
#############

# TODO

#############
## Testing ##
#############

# TODO
