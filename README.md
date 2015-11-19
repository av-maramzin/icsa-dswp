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

### Testing

Run `tools/test.sh`. This script will run the unit tests for the project.

Development
-----------

### Disclaimer

The following text would describe the repository when the implementation is
completed - it may not describe its working state.

### Structure

The `pass` directory contains an LLVM 3.7.0 loop pass that implements DSWP.

The `test` directory contains example programs on which the DSWP pass can be
tested and demonstrated. Run `tools/test.py` in order to test that the pass
maintains the semantics of the test programs.


[ottoni2005]: (http://dl.acm.org/citation.cfm?id=1100543)
[2011-dswp-prj]: (http://www.cs.cmu.edu/~fuyaoz/courses/15745/)
[2013-dswp-prj]: (http://www.cs.cmu.edu/~avelingk/compilers/)


