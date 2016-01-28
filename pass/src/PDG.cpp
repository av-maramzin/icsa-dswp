#include "PDG.h"

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

#include "llvm/ADT/GraphTraits.h"
using llvm::GraphTraits;

#include "llvm/ADT/SmallVector.h"
using llvm::SmallVector;

#include "Util.h"

namespace icsa {

char ProgramDependenceGraphPass::ID = 0;
RegisterPass<ProgramDependenceGraphPass>
    PDGRegister("pdg", "Build Program Dependence Graph");


bool ProgramDependenceGraphPass::runOnFunction(Function &F) {
  typedef GraphTraits<const ControlDependenceGraph *> CDGTraits;
  typedef GraphTraits<const DataDependenceGraph *> DDGTraits;
  PDG.firstValue = F.getEntryBlock().begin();

  ControlDependenceGraphPass &cdgp = Pass::getAnalysis<ControlDependenceGraphPass>();
  MemoryDependenceGraphPass &mdgp = Pass::getAnalysis<MemoryDependenceGraphPass>();
  DataDependenceGraphPass &ddgp = Pass::getAnalysis<DataDependenceGraphPass>();

  ddg_iterator ddg_begin = DDGTraits::nodes_begin(&ddgp.getDDG());
  ddg_iterator ddg_end = DDGTraits::nodes_end(&ddgp.getDDG());

  mdg_iterator mdg_begin = DDGTraits::nodes_begin(&mdgp.getMDG());
  mdg_iterator mdg_end = DDGTraits::nodes_end(&mdgp.getMDG());

  cdg_iterator cdg_begin = CDGTraits::nodes_begin(&cdgp.getCDG());
  cdg_iterator cdg_end = CDGTraits::nodes_end(&cdgp.getCDG());

  // Add nodes.
  for (ddg_iterator ddg_it = ddg_begin; ddg_it != ddg_end; ++ddg_it) {
    PDG.addNode(ddg_it->getValue());
  }

  // Add edges.
  for (cdg_iterator cdg_it = cdg_begin; cdg_it != cdg_end; ++cdg_it) {
    Instruction &source = cdg_it->getValue()->back();
    vector<BasicBlock *> deps = cdgp.getCDG().getDependants(cdg_it->getValue());
    for (BasicBlock *BB : deps) {
      for (BasicBlock::iterator it = BB->begin(); it != BB->end(); ++it) {
        PDG.addEdge(&source, &*it);
      }
    }
  }

  for (ddg_iterator ddg_it = ddg_begin; ddg_it != ddg_end; ++ddg_it) {
    Instruction *source = ddg_it->getValue();
    vector<Instruction *> deps = ddgp.getDDG().getDependants(ddg_it->getValue());
    for (Instruction *I : deps) {
      PDG.addEdge(source, I);
    }
  }

  for (ddg_iterator mdg_it = mdg_begin; mdg_it != mdg_end; ++mdg_it) {
    Instruction *source = mdg_it->getValue();
    vector<Instruction *> deps = mdgp.getMDG().getDependants(mdg_it->getValue());
    for (Instruction *I : deps) {
      PDG.addEdge(source, I);
    }
  }

  return true;
}

}
