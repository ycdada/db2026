cmake_minimum_required(VERSION 3.16)
project(RMDB)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)

add_compile_options(-Wall)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb3" CACHE STRING "Flags used by the CXX compiler during Debug builds." FORCE)
set(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE STRING "Flags used by the C compiler during Debug builds." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Flags used by the CXX compiler during Release builds." FORCE)
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Flags used by the C compiler during Release builds." FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -ggdb3 -DNDEBUG -fno-omit-frame-pointer" CACHE STRING "Flags used by the CXX compiler during RelWithDebInfo builds." FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -g -DNDEBUG -fno-omit-frame-pointer" CACHE STRING "Flags used by the C compiler during RelWithDebInfo builds." FORCE)


enable_testing()
add_subdirectory(src)
add_subdirectory(deps)
