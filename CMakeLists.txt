cmake_minimum_required(VERSION 3.13) #CMake version check
project(Music_Recognition) #Create the projet

set(CMAKE_CXX_STANDARD 14) #Stand version used

#set(SOURCE_FILES src/Application.cpp) #Definition of SOURCE_FILES

file(GLOB SOURCES src/*.h src/*.cpp) #Loading all files (change if cmake should not by using top command, and changing bot one)

add_executable(Music_Recognition ${SOURCES} ) #Target of executable
