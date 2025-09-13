cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

set(TARGET_NAME    "rmab")
project(${TARGET_NAME} CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

set(SOURCE_FILES
    src/dp.cpp
    src/matrix.cpp
    src/io.cpp
    src/solve.cpp
    src/prob.cpp
)

add_executable(RMAB main.cpp ${SOURCE_FILES})

target_precompile_headers(RMAB PRIVATE
    include/dp.h
    include/matrix.h
    include/rmab.h
    include/utils.h
    include/prob.h
)

target_include_directories(RMAB PRIVATE include)

target_compile_options(RMAB PRIVATE -Ofast -march=native -ffast-math -flto=auto -DNDEBUG -fno-exceptions)
target_link_options(RMAB PRIVATE -Ofast -march=native -ffast-math -flto=auto -DNDEBUG -fno-exceptions)