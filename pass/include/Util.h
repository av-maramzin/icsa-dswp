#ifndef ICSA_DSWP_UTIL_H
#define ICSA_DSWP_UTIL_H

#include <string>
using std::string;

#include "llvm/IR/Instruction.h"
using llvm::Instruction;
#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;

namespace icsa {

string instructionToFunctionName(const Instruction &Inst);
string basicBlockToFunctionName(const BasicBlock &BB);

}

#endif
