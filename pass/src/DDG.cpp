#include "DDG.h"

#include <vector>
using std::vector;
#include <set>
using std::set;
#include <utility>
using std::move;

#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/User.h"
using llvm::User;
#include "llvm/IR/InstIterator.h"
using llvm::inst_iterator;

#include "llvm/PassSupport.h"
using llvm::RegisterPass;

#include "llvm/Support/Casting.h"
using llvm::dyn_cast;

namespace icsa {

char DataDependenceGraphPass::ID = 0;
RegisterPass<DataDependenceGraphPass>
    DDGRegister("ddg", "Build Data Dependence Graph");

bool DataDependenceGraphPass::runOnFunction(Function &F) {
  // Add nodes.
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    Instruction *Inst = &*I;
    DDG.addNode(Inst);
  }

  // Add edges.
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    // Every user of an instruction is its user.
    for (User *U : I->users()) {
      if (Instruction *Inst = dyn_cast<Instruction>(U)) {
        DDG.addEdge(&*I, Inst);
      }
    }
  }

  return false;
}
}
