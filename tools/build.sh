#! /bin/bash

SELF_DIR=$(dirname $0)
PROJECT_ROOT=${PWD}/${SELF_DIR}/../

#

echo "project root dir: ${PROJECT_ROOT}"

mkdir -p build
cd build

CC=clang \
cmake ${PROJECT_ROOT}
make

