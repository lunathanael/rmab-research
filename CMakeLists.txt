cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(RMAB CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(RMAB main.cpp)

target_include_directories(RMAB PRIVATE include)

target_compile_options(RMAB PRIVATE -Ofast -march=native -ffast-math -flto -DNDEBUG -fno-exceptions)