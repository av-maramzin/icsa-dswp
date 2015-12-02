// Implemented in Util.cpp.

#ifndef ICSA_DSWP_UTIL_H
#define ICSA_DSWP_UTIL_H

#include <string>
using std::string;

#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;

namespace icsa {

string getBBSourceCode(const BasicBlock &BB);

}

#endif // ICSA_DSWP_UTIL_H
