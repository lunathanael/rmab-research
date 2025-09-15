cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

set(TARGET_NAME    "rmab")
project(${TARGET_NAME} CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

find_package(OpenMP REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_FLAGS_PROFILE "-O3 -g -march=native -ffast-math -flto=auto -fno-exceptions -DNDEBUG -pg ${OpenMP_CXX_FLAGS}" CACHE STRING "Flags used by the Profile build type" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_PROFILE "-O3 -g -march=native -ffast-math -flto=auto -fno-exceptions -DNDEBUG -pg ${OpenMP_CXX_FLAGS}" CACHE STRING "Linker flags for Profile build type" FORCE)

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native -ffast-math -flto=auto -fno-exceptions -DNDEBUG ${OpenMP_CXX_FLAGS}" CACHE STRING "Flags used by the Release build type" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Ofast -march=native -ffast-math -flto=auto -fno-exceptions -DNDEBUG ${OpenMP_CXX_FLAGS}" CACHE STRING "Linker flags for Release build type" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g ${OpenMP_CXX_FLAGS}" CACHE STRING "Flags used by the Debug build type" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${OpenMP_CXX_FLAGS}" CACHE STRING "Linker flags for Debug build type" FORCE)

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -march=native -ffast-math -flto=auto -fno-exceptions -DNDEBUG ${OpenMP_CXX_FLAGS}" CACHE STRING "Flags used by the RelWithDebInfo build type" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "-O2 -g -march=native -ffast-math -flto=auto -fno-exceptions -DNDEBUG ${OpenMP_CXX_FLAGS}" CACHE STRING "Linker flags for RelWithDebInfo build type" FORCE)

set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -march=native -ffast-math -flto=auto -fno-exceptions -DNDEBUG ${OpenMP_CXX_FLAGS}" CACHE STRING "Flags used by the MinSizeRel build type" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "-Os -march=native -ffast-math -flto=auto -fno-exceptions -DNDEBUG ${OpenMP_CXX_FLAGS}" CACHE STRING "Linker flags for MinSizeRel build type" FORCE)

set(SOURCE_FILES
    src/dp.cpp
    src/matrix.cpp
    src/io.cpp
    src/solve.cpp
    src/prob.cpp
    src/utils.cpp
)

add_executable(RMAB main.cpp ${SOURCE_FILES})

target_precompile_headers(RMAB PRIVATE
    include/utils.h
    include/dp.h
    include/matrix.h
    include/rmab.h
    include/utils.h
    include/prob.h
)

target_include_directories(RMAB PRIVATE include)

target_link_libraries(RMAB PRIVATE OpenMP::OpenMP_CXX)

if(CMAKE_BUILD_TYPE STREQUAL "Profile")
    message(STATUS "Configuring for Profile build type")
endif()