cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(clogreader CXX)

set(SOURCES 
src/logreader/CLogReader.h
src/logreader/CLogReader.cpp
src/tools/MyString.h
src/regex/MyRegex.h
src/regex/MyRegex.cpp
src/regex/Matcher.h
src/regex/Matcher.cpp
)
add_library(logreader ${SOURCES})

add_executable(clogreader src/main/main.cpp)
target_link_libraries(clogreader logreader)

enable_testing()

add_subdirectory(tests)