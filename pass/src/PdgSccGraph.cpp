#include "PdgSccGraph.h"

#include <iostream>
using std::cout;
using std::endl;

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

#include "llvm/ADT/GraphTraits.h"
using llvm::GraphTraits;

#include "llvm/ADT/SmallVector.h"
using llvm::SmallVector;

#include "llvm/IR/Instructions.h"
using llvm::PHINode;
using llvm::Constant;
using llvm::BranchInst;

#include "PDG.h"
#include "GraphUtils.h"

namespace icsa {

char PdgSccGraphPass::ID = 0;
RegisterPass<PdgSccGraphPass>
    PdgSccGraphPassRegister("psg",
                            "Build PDG Strongly Connected Components Graph");

bool PdgSccGraphPass::runOnFunction(Function &F) {
  typedef GraphTraits<const ProgramDependenceGraph *> PDGTraits;
  typedef GraphTraits<const PdgSccGraph *> PSGTraits;
  typedef GraphUtils<ProgramDependenceGraph> GUtils;

  ProgramDependenceGraphPass &pdgp =
      Pass::getAnalysis<ProgramDependenceGraphPass>();

  PSG.firstValue = nullptr;

  auto sccs = GUtils::findSCC(pdgp.getPDG());
  cout << "sccs.size() = " << sccs.size() << endl;

  for (auto p : sccs) {
    cout << p.first << ":";
    for (auto dep : p.second) {
      cout << ' ' << dep;
    }
    cout << endl;
  }

  for (auto p : sccs) {
    SCCs.push_back(new vector<Instruction *>());
    for (auto dep : p.second) {
      SCCs.back()->push_back(dep->getValue());
    }
    cout << "[DEBUG] SCCs.back() = " << SCCs.back() << endl;
    PSG.addNode(SCCs.back());

    if (nullptr == PSG.firstValue) {
      PSG.firstValue = SCCs.back();
    }
  }

  // Can be optimized...
  for (PSGTraits::nodes_iterator I = PSGTraits::nodes_begin(&PSG),
                                 E = PSGTraits::nodes_end(&PSG);
       I != E; ++I) {
    for (PSGTraits::nodes_iterator J = PSGTraits::nodes_begin(&PSG),
                                   F = PSGTraits::nodes_end(&PSG);
         J != F; ++J) {
      if (I == J) continue;
      vector<Instruction *> &froms = *I->getValue();
      for (auto inst_from : froms) {
        vector<Instruction *> &tos = *J->getValue();
        for (auto inst_to : tos) {
          if (pdgp.getPDG().dependsOn(inst_from, inst_to)) {
            PSG.addEdge(**I, **J);
            goto next_scc; 
          }
        }
      }
next_scc: ;
    }
  }

  return false;
}
}
