#include "PSG.h"

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

#include "Dependence.h"
using icsa::DependenceGraph;

#include "GraphUtils.h"
using icsa::GraphUtils;

namespace icsa {

char PDGSCCGraphPass::ID = 0;
RegisterPass<PDGSCCGraphPass>
    PSGRegister("psg", "Build PDG Strongly Connected Components Graph");

bool PDGSCCGraphPass::runOnFunction(Function &F) {
  typedef GraphUtils<Instruction> GUtils;

  DependenceGraph<Instruction> PDG =
      Pass::getAnalysis<ProgramDependenceGraphPass>().getPDG();

  map<const Instruction *, const Instruction *> component =
      GUtils::findSCC(PDG);

  map<Instruction *, set<Instruction *>> result;
  for (const auto &pair : component) {
    SCCs[pair.second];
    SCCs[pair.second].insert(pair.first);
  }

  for (const auto &pair : SCCs) {
    PSG.addNode(&pair.second);
  }

  for (const auto &pair : component) {
    for (auto I = PDG.child_cbegin(pair.first), E = PDG.child_cend(pair.first);
         I != E; ++I) {
      PSG.addEdge(&SCCs[pair.second], &SCCs[component.at(*I)]);
    }
  }

  return false;
}
}
