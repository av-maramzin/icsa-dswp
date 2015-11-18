cmake_minimum_required(VERSION 2.8.12.2)
project(dswp-runtime)

file(GLOB RUNTIME_SOURCES "src/*.c")

add_library(runtime STATIC ${RUNTIME_SOURCES})

target_include_directories(runtime PUBLIC "include/")
