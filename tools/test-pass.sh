#! /bin/bash
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 PASS_TO_TEST TEST_CPP_FILE" >&2
  exit 1
fi
if ! [ -e "$2" ]; then
  echo "$2 not found" >&2
  exit 1
fi
if ! [ -f "$2" ]; then
  echo "$2 not a file" >&2
  exit 1
fi

SELF_DIR=$(dirname $0)
PROJECT_ROOT=${PWD}/${SELF_DIR}/../

echo "Compiling $2 to $2.bc"
clang++ -g -std=c++11 -c -emit-llvm $2 -o $2.bc

echo "Converting $2.bc to SSA form in $2.ssa.bc"
opt -mem2reg $2.bc -o $2.ssa.bc

echo "Running $1 pass"
opt -load ${PROJECT_ROOT}/build/pass/libdswp.so $1 $2.ssa.bc -o /dev/null

if [ "$?" -ne 0 ]; then
  echo "Result: FAILED"
else
  echo "Result: SUCCESS"
fi

echo "Removing $2.bc and $2.ssa.bc"
rm $2.bc $2.ssa.bc
