# Author: Hakan Yıldız
# Shared under MIT License. See the file LICENSE for more info.

cmake_minimum_required(VERSION 3.21)

project("vplc" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(compiler-options INTERFACE)

target_compile_options(compiler-options INTERFACE
                       $<$<C_COMPILER_ID:MSVC>:/W4>
                       $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
                       $<$<C_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>)

add_executable(diff_checker_char diff_checker_char.cpp Tokenizer.ipp diff_checker_base.ipp)
target_link_libraries(diff_checker_char PRIVATE compiler-options)

add_executable(diff_checker_real diff_checker_real.cpp Tokenizer.ipp diff_checker_base.ipp)
target_link_libraries(diff_checker_real PRIVATE compiler-options)
