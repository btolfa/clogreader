cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(logreader-test)

add_executable(logreader-test LogReaderTest.cpp)
target_link_libraries(logreader-test logreader gtest gmock gmock_main)

add_test(NAME LogReaderTest COMMAND logreader-test)