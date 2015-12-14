#include "MDG.h"

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
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
using llvm::MemoryDependenceAnalysis;
#include "llvm/IR/Function.h"
using llvm::Function;

#include "Util.h"

namespace icsa {

char MemoryDependenceGraphPass::ID = 0;
RegisterPass<MemoryDependenceGraphPass>
    MDGRegister("mdg", "Build Memory Dependence Graph");

bool MemoryDependenceGraphPass::runOnFunction(Function &F) {
  MDG.firstValue = F.getEntryBlock().begin();

  MemoryDependenceAnalysis &mda = Pass::getAnalysis<MemoryDependenceAnalysis>();

  return true;
}

}
