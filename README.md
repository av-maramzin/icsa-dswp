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

### Code Structure

This section talks about the source code of the project. It is meant to be read
while looking at the source code, so go ahead and open an editor.

#### Dependence.h

The starting point for understanding the code is the `Dependence.h` header file.
It implements the template class `DependenceGraph<ValueType>`. This class
represents dependences between abstract objects of type `ValueType` - let's call
these objects 'nodes' for the sake of discussion. The dependences are
represented as a map from nodes to sets of nodes that depend on them.

It is worth noting that nodes are not stored in the Graph, but elsewhere, and
are referenced by using pointers to them. For example, if the node `Value` has
type `ValueType *` then internally for the `DependenceGraph` class the nodes
which depend on `Value` are stored in the set accessed as `Nodes[Value]`. The
type of this set is `std::set<ValueType *>`. In other words, the type of `Nodes`
is `std::map<ValueType*, std::set<ValueType *>>`.

There is a common sense interface exposed by `DependenceGraph<ValueType>`,
allowing users to add and remove nodes, add edges, check if a node depends on
another one, and clear the graph. There are also `begin` and `end` iterators
which allow the traversal of the nodes of the graph, and are nothing but aliases
for the `begin` and `end` iterators of the underlying `map` structure. This
allows the user to corrupt the `DependenceGraph` structure, which we hope they
won't do.

The type of the node iterators are `std::map<ValueType *, std::set<ValueType
*>>::iterator` (aliased to `nodes_iterator`), thus in order to traverse the
dependants of a single node, one needs to use `iterator->second.begin()` and
`iterator->second.end()`. The type of these iterators is `std::set<ValueType
*>::iterator` (aliased to `dependant_iterator`).

In order to test this class, compile and run `pass/test/TestDependence.cpp`.

### DDG.h and DDG.cpp

The simplest use of the `DependenceGraph` class is in the implementation of the
Data Dependence Graph (DDG) LLVM function pass, which builds the DDG of a
function using the def-use chains of instructions. The header file `DDG.h`
defines a trivial `FunctionPass` - `DataDependenceGraphPass`. The non-trivial
parts of the definition are that it is storing an instance of
`DependenceGraph<Instruction>` which can be accessed via a getter method and
cleared via `releaseMemory()`, and the declaration inside `getAnalysisUsage()`
that the pass doesn't modify the given `LLVM IR` in any way.

The implementation of the pass is in the source file `DDG.cpp`. It includes some
LLVM boilerplate (registering the pass), but the entry point is the
`runOnFunction` method. It first loops over the instructions of a function and
adds them to the internal (for the pass) `DependenceGraph` as nodes. Then it
goes over the instructions again and adds all of their 'users' from their
def-use chain as nodes dependant on the instructions in the `DependenceGraph`.

In order to test this pass, build the project and execute `tools/test-ddg.sh`
with a test `cpp` file as a command line argument.

This pass is useless on its own, as there is no way to extract its result. For
this reason, we need to look at `DDGPrinter.h` and `DDGPrinter.cpp`.

[ottoni2005]: (http://dl.acm.org/citation.cfm?id=1100543)
[2011-dswp-prj]: (http://www.cs.cmu.edu/~fuyaoz/courses/15745/)
[2013-dswp-prj]: (http://www.cs.cmu.edu/~avelingk/compilers/)
[cytron1989]: (http://dl.acm.org/citation.cfm?id=75280)

