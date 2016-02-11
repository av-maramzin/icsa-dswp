#include "Util.h"

#include <string>
using std::string;

#include "llvm/IR/Instruction.h"
using llvm::Instruction;
#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;
#include "llvm/IR/Function.h"
using llvm::Function;

namespace icsa {

string instructionToFunctionName(const Instruction &Inst) {
  string FuncName = "unknown";
  if (const BasicBlock *BB = Inst.getParent()) {
    if (const Function *Func = BB->getParent()) {
      FuncName = Func->getName();
    }
  }
  return FuncName;
}

}
