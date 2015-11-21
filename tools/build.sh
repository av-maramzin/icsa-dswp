#! /bin/bash

SELF_DIR=$(dirname $0)
PROJECT_ROOT=${PWD}/${SELF_DIR}/../

#

echo "project root dir: ${PROJECT_ROOT}"

mkdir -p build
cd build


# notes on cmake call

#- CC/CXX env variables dictate the compiler cmake should use
#- LLVM_DIR provides a hint for cmake's find_package for LLVM
#- CMAKE_EXPORT_COMPILE_COMMANDS exports the project's compilation db
#  (e.g. use with YCM)

CC=clang CXX=clang++ \
cmake \
  -DLLVM_DIR=$(llvm-config --prefix)/share/llvm/cmake/ \
  -DCMAKE_BUILD_TYPE=DEBUG \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_INSTALL_PREFIX=../install \
  ${PROJECT_ROOT}

make

