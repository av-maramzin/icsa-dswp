#ifndef ICSA_DSWP_UTIL_H
#define ICSA_DSWP_UTIL_H

#include <string>
using std::string;

#include "llvm/IR/Instruction.h"
using llvm::Instruction;

namespace icsa {

string instructionToFunctionName(const Instruction &Inst);

}

#endif
