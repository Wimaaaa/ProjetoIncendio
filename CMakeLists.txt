cmake_minimum_required(VERSION 3.29)
project(projeto_floresta C)

set(CMAKE_C_STANDARD 11)

add_executable(projeto_floresta main.c
        functions.c)
