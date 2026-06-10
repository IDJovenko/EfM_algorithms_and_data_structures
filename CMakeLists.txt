cmake_minimum_required(VERSION 4.2)

project(EfM_Module_3 LANGUAGES CXX)

set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources/data")

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
if(MSVC)
    add_compile_options(/W4)    
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

enable_testing()

add_subdirectory(src/task1_containers)
