An implementation of DSWP
=========================

Currently, under development.

Goals
-----

Decoupled Software Pipelining (DSWP) is an automatic thread extraction technique
([Ottoni, 2005][ottoni2005]). There have been previous attempts at implementing
it ([2011 project][2011-dswp-prj] and [2013 project][2013-dswp-prj]), but the
efforts resulted in unfinished and unmaintainable solutions.

Therefore, the goals of this project are to provide a complete, reusable and
maintainable implementation of the technique, which can then be integrated in
prototypes of further research on the topic.

Usage
-----

### Prerequisites

This project depends on LLVM+Clang 3.7.0 and is developed on Linux. Thus, no
guarantees are made that it's going to work with any other version of LLVM or on
any other operating system - sorry. Make sure you have the LLVM+Clang 3.7.0
binary directory on your system PATH in order to build and use the pass.

### Building

Run `tools/build.sh`. This script will use `cmake 2.8` and `make` in order to
build the project.

Development
-----------

### Disclaimer

The following text would describe the repository when the implementation is
completed - it may not describe its working state.

### Available Passes

The `pass` directory contains the code for `libdswp.so` - a library that
contains the following LLVM 3.7.0 function passes:
 * `ddg` - builds the Data Dependence Graph of a function using the Use-Def
   chains already built-in LLVM; in order for this pass to work, the bytecode
   should be in SSA form;
 * `mdg` - builds the Memory Dependence Graph of a function using
   `llvm/Analysis/MemoryDependenceAnalysis.h`; only instruction definition
   dependencies are considered (no clobber/alias dependencies);
 * `cdg` - builds the [Control Dependence Graph][cytron1989] of a function;
 * `pdg` - combines the previous three graphs in a single graph of instruction
   dependencies; note that since `cdg` produces a basic block dependence graph
   all of the instructions in the dependant block are made to depend on the
   branch instruction of the dependency source;
 * `psg` - incomplete and memory leaking experimental pass; finds the Strongly
   Connected Components of the Program Dependence Graph and builds a graph out
   of them;
 * `dot-XXX` - prints a .dot representation of each function for the graph build
   by `XXX` to a file; `XXX` is one of the five graph building passes;
 * `find-dswp` - uses the experimental `psg` pass to analyse an LLVM bytecode
   file and prints DSWP opportunities to the standard output.

In order to analyze a `.cpp` file called `foo.cpp`, make sure you have LLVM
3.7.0 installed and do the following:

```
clang++ foo.cpp -c -emit-llvm -o foo.bc
opt -mem2dep foo.bc -o ssa.bc
opt -load $ICSA_DSWP_HOME/build/pass/libdswp.so -find-dswp ssa.bc -o /dev/null
```

The `test` directory contains example programs on which the DSWP pass can be
tested and demonstrated.

[ottoni2005]: (http://dl.acm.org/citation.cfm?id=1100543)
[2011-dswp-prj]: (http://www.cs.cmu.edu/~fuyaoz/courses/15745/)
[2013-dswp-prj]: (http://www.cs.cmu.edu/~avelingk/compilers/)
[cytron1989]: (http://dl.acm.org/citation.cfm?id=75280)

