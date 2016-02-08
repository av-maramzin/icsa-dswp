#! /bin/bash
for SRC in pass/src/*.cpp
do
  echo "Generating .bc for ${SRC}"
  clang++ -std=c++11 -g -O0 -c -emit-llvm -Ipass/include -I/usr/local/include \
    -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS $SRC -o /tmp/tmp.bc 
  echo "Promoting memory allocation to registers"
  opt -mem2reg /tmp/tmp.bc -o /tmp/tmp.ssa.bc
  echo "Searching for DSWP opportunities"
  opt -load build/pass/libdswp.so -find-dswp /tmp/tmp.ssa.bc -o /dev/null
done
