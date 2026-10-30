cmake_minimum_required(VERSION 3.18)
project(hooktnt)

add_library(hooktnt SHARED main.cpp And64InlineHook.cpp)

target_link_libraries(hooktnt log android dl)
