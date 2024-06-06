cmake_minimum_required(VERSION 3.10)
project(fluffy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(fluffy src/main.cpp src/sqlite3.c)