cmake_minimum_required(VERSION 3.10)
project(word_search C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(wordsearch main.c)
