cmake_minimum_required(VERSION 3.0)
project(stb_wrapper)

option(BUILD_TEST "build unit test" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install/)

add_subdirectory(src)
if(BUILD_TEST)
    add_subdirectory(test)
else()
    add_subdirectory(test EXCLUDE_FROM_ALL)
endif()