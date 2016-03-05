cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(tests)

set(gtest_force_shared_crt ON)
add_subdirectory(googletest)
add_subdirectory(fileops)