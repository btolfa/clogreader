cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(fileops-test)

add_executable(fileops-test FileTest.cpp)
target_link_libraries(fileops-test logreader gtest gmock gmock_main)

add_test(NAME FileTest COMMAND fileops-test)