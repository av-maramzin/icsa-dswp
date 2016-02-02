#include "DDG.h"

#include <iostream>
using std::cout;

#include <memory>
using std::unique_ptr;
#include <map>
using std::map;
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <utility>
using std::move;

#include "llvm/PassSupport.h"
using llvm::RegisterPass;
#include "llvm/IR/InstIterator.h"
using llvm::inst_iterator;
#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/User.h"
using llvm::User;

#include "llvm/Support/Casting.h"
using llvm::dyn_cast;

#include "Util.h"

namespace icsa {

char DataDependenceGraphPass::ID = 0;
RegisterPass<DataDependenceGraphPass>
    DDGRegister("ddg", "Build Data Dependence Graph");

bool DataDependenceGraphPass::runOnFunction(Function &F) {
  DDG.firstValue = F.getEntryBlock().begin();

  // Add nodes.
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    const Instruction& Inst = *I;
    DDG.addNode(const_cast<Instruction*>(&Inst));
  }

  // Add edges.
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    for (User *U : I->users()) {
      if (Instruction *Inst = dyn_cast<Instruction>(U)) {
        DDG.addEdge(DDG.find(const_cast<Instruction*>(&*I)), DDG.find(Inst));
      }
    }
  }

  return false;
}

}
