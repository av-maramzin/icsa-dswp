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
  // Finish adding elements to SCCs, to ensure the memory layout won't change
  // once we start using the internal pointers of the structure.

  for (const auto &pair : component) {
    const set<const Instruction *> *SCC = &SCCs.at(pair.second);
    InstToSCC[pair.first] = SCC;
    PSG.addNode(SCC);
  }

  for (const auto &pair : component) {
    for (auto I = PDG.child_cbegin(pair.first), E = PDG.child_cend(pair.first);
         I != E; ++I) {
      auto SCC1 = &SCCs[pair.second];
      auto SCC2 = &SCCs[component.at(*I)];
      // Don't add edges from nodes to themselves: this information is
      // implicitly stored in the number of elements SCCs have: if they are
      // more than 1, then the SCC obviously depends on itself.
      if (SCC1 != SCC2) {
        PSG.addEdge(SCC1, SCC2);
      }
    }
  }

  return false;
}
}
