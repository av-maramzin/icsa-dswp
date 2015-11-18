cmake_minimum_required(VERSION 2.8.12.2)
project(dswp-pass)

file(GLOB DSWP_SOURCES src/*.cpp)

add_library(dswp MODULE ${DSWP_SOURCES})

target_include_directories(dswp PUBLIC "include")

target_link_libraries(dswp ${llvm_libs})
