#! /bin/bash

SELF_DIR=$(dirname $0)
PROJECT_ROOT=${PWD}/${SELF_DIR}/../

clang++ -c -emit-llvm $1 -o $1.bc

opt -mem2reg $1.bc -o $1.ssa.bc

opt -load build/pass/libdswp.so -dot-ddg $1.ssa.bc
opt -load build/pass/libdswp.so -dot-mdg $1.ssa.bc
opt -load build/pass/libdswp.so -dot-cdg $1.ssa.bc
opt -load build/pass/libdswp.so -dot-pdg $1.ssa.bc

dot -Tpdf ddg.main.dot -o ddg.pdf
dot -Tpdf cdg.main.dot -o cdg.pdf
dot -Tpdf mdg.main.dot -o mdg.pdf
dot -Tpdf pdg.main.dot -o pdg.pdf

