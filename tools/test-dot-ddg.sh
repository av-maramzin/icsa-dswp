#! /bin/bash
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 TEST_CPP_FILE" >&2
  exit 1
fi
if ! [ -e "$1" ]; then
  echo "$1 not found" >&2
  exit 1
fi
if ! [ -f "$1" ]; then
  echo "$1 not a file" >&2
  exit 1
fi

SELF_DIR=$(dirname $0)
PROJECT_ROOT=${PWD}/${SELF_DIR}/../

echo "Compiling $1 to $1.bc"
clang++ -std=c++11 -c -emit-llvm $1 -o $1.bc

echo "Converting $1.bc to SSA form in $1.ssa.bc"
opt -mem2reg $1.bc -o $1.ssa.bc

echo "Running -dot-ddg pass"
opt -load ${PROJECT_ROOT}/build/pass/libdswp.so -dot-ddg $1.ssa.bc -o /dev/null

if [ "$?" -ne 0 ]; then
  echo "Result: FAILED"
else
  echo "Result: SUCCESS"
fi

echo "Removing $1.bc and $1.ssa.bc"
rm $1.bc $1.ssa.bc
