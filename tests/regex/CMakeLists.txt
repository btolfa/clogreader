cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(regex-test)

add_executable(regex-test RegexTest.cpp)
target_link_libraries(regex-test logreader gtest gmock gmock_main)

add_test(NAME RegexTest COMMAND regex-test)