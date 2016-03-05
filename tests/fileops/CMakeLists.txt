cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(fileops_test)

add_executable(fileops_test FileTest.cpp)
target_link_libraries(fileops_test logreader gtest gmock gmock_main)

add_test(NAME FileTest COMMAND fileops_test)