cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(clogreader CXX)

set(SOURCES 
src/main/main.cpp
src/logreader/CLogReader.h
src/logreader/CLogReader.cpp
)


add_executable(clogreader ${SOURCES})
