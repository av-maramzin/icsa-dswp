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
another one, and clear the graph. There are also `nodes_begin` and `nodes_end`
iterators which allow the traversal of the nodes of the graph, and are nothing
but aliases for the `begin` and `end` iterators of the underlying `map`
structure. This allows the user to corrupt the `DependenceGraph` structure,
which we hope they won't do.

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
this reason, we need to look at `DDGPrinter.cpp`.

### DDGPrinter.cpp and GraphTraits

The DDG of a function can be printed using the `DDGPrinter` pass, defined in
`DDGPrinter.cpp`. It depends on the `DataDependenceGraphPass` as specified in
`getAnalysisUsage` and it uses the `DepGraphTraitsWrapper` class defined in
`DependenceTraits.h` in order to print the graph using the LLVM `GraphWriter`
class. Also, `InstDOTTraits.h` is included as it defines `DOTGraphTraits` for
`DependenceGraph<Instruction>`, which is used by `GraphWriter` to decide how to
label the nodes of the graph. This is specified in the `getNodeLabel` method of
`DOTGraphTraits` and it just prints the instruction as text.

It is worth discussing the structure of the classes defined in
`DependenceTraits.h`. In order to define LLVM's `GraphTraits` for a graph
structure, it needs to be represented by separate classes - one for the graph
and one for nodes in the graph. However, we decided to implement the graph as a
single class, as described in the `Dependence.h` section. For this reason, we
had to implement wrappers around it in order to comply with the requirements of
`GraphTraits`.

When specializing `GraphTraits` for a graph type, the following elements need to
be defined:

 * `NodeType` - the class that represents a node of the graph;
 * `nodes_iterator` - an iterator that is constructed from a constant reference
   to a graph object and dereferences to a pointer to a node object;
 * `nodes_begin` and `nodes_end` - methods that construct the begin and end
   `nodes_iterator`s given a constant reference to a graph object;
 * `ChildIteratorType` - an iterator that is constructed from a pointer to a
   node object and is dereferenced to a pointer to a node object;
 * `child_begin` and `child_end` - methods that construct the begin and end
   `ChildIteratorType`s given a pointer to a node object.

This is done in the `GraphTraits<DepGraphTraitsWrapper<ValueType>>` class
defined in `DependenceTraits.h`.

The `DepGraphTraitsWrapper` class that `GraphTraits` is being specialized for,
needs to be constructed out of a `DependenceGraph` object. The constructor
allocates a `DepNodeTraitsWrapper` for every node in the given
`DependenceGraph` and stores a `std::map` from `ValueType *`s to these new
`DepNodeTraitsWrapper`s. Let's call the node type `Node` and the graph type
`Graph` for the sake of discussion. `Node` stores a reference to the `Graph`
that contains it, a constant `ValueType *` to the node it represents, and a
const pointer to `std::set<ValueType *>` - the set of the nodes that depend on
it. In this way, `Node` doesn't own any of the objects it references, but is
simply a proxy used to plug into the `GraphTraits` mechanism.

There are two more types defined in `DependenceTraits.h`: `DepNodeIterator` and
`DepNodeChildIterator`. These are the required `nodes_iterator` and
`ChildIteratorType` required by `GraphTraits`. The interesting functions in the
implementation are the constructors and the dereferencing operators
(`operator*`).

`DepNodeIterator` is constructed from a `const_iterator` of the map
specialization used by `Graph`, which dereferences to a `std::pair<ValueType *,
std::unique_ptr<Node>>` and thus the `DepNodeIterator` is dereferenced by taking
the second element of that pair and extracting the pointer that it manages.
Explicitly, this looks like `idx->second.get()`, where `idx` is the previously
mentioned `const_iterator`.

`DepNodeChildIterator` is constructed from an `iterator` of the set that is the
`value_type` of the map used in `DependenceGraph`, namely `std::set<ValueType
*>`. A reference to a `DepGraphTraitsWrapper` is also provided during
construction of the `DepNodeChildIterator`, as in order to dereference this
iterator to a `Node`, the `Graph` storing that `Node` needs to be known. This is
also the reason `Node`s store a reference to their containing `Graph` - in order
to pass it to the constructor of their child iterators. This iterator is
dereferenced by first dereferencing the stored set iterator and then using the
value to extract the `Node` that the `Graph` associates with this value. Namely,
`G[*idx]`, where `idx` is the previously mentioned `iterator` and `G` is an
instance of `Graph`.

All of this comes together when invoking `llvm::WriteGraph` in the method
`writeToFile` in the `Graph` class definition. By using the specialization of
`GraphTraits` `llvm::WriteGraph` knows how to traverse our custom graph and
write it to a file in the `.dot` format.

[ottoni2005]: (http://dl.acm.org/citation.cfm?id=1100543)
[2011-dswp-prj]: (http://www.cs.cmu.edu/~fuyaoz/courses/15745/)
[2013-dswp-prj]: (http://www.cs.cmu.edu/~avelingk/compilers/)
[cytron1989]: (http://dl.acm.org/citation.cfm?id=75280)

