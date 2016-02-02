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

#include "llvm/IR/Instructions.h"
using llvm::PHINode;
using llvm::Constant;
using llvm::BranchInst;

#include "Util.h"

namespace icsa {

char ProgramDependenceGraphPass::ID = 0;
RegisterPass<ProgramDependenceGraphPass>
    PDGRegister("pdg", "Build Program Dependence Graph");

bool ProgramDependenceGraphPass::runOnFunction(Function &F) {
  typedef GraphTraits<const ControlDependenceGraph *> CDGTraits;
  typedef GraphTraits<const DataDependenceGraph *> DDGTraits;
  typedef GraphTraits<const MemoryDependenceGraph *> MDGTraits;
  typedef GraphTraits<const ProgramDependenceGraph *> PDGTraits;

  typedef typename DDGTraits::ChildIteratorType ddg_child_iterator;

  PDG.firstValue = F.getEntryBlock().begin();

  ControlDependenceGraphPass &cdgp =
      Pass::getAnalysis<ControlDependenceGraphPass>();
  MemoryDependenceGraphPass &mdgp =
      Pass::getAnalysis<MemoryDependenceGraphPass>();
  DataDependenceGraphPass &ddgp = Pass::getAnalysis<DataDependenceGraphPass>();

  ddg_iterator ddg_begin = DDGTraits::nodes_begin(&ddgp.getDDG());
  ddg_iterator ddg_end = DDGTraits::nodes_end(&ddgp.getDDG());

  // Add nodes.
  for (ddg_iterator ddg_it = ddg_begin; ddg_it != ddg_end; ++ddg_it) {
    PDG.addNode(ddg_it->getValue());
  }

  cdg_iterator cdg_begin = CDGTraits::nodes_begin(&cdgp.getCDG());
  cdg_iterator cdg_end = CDGTraits::nodes_end(&cdgp.getCDG());

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
    vector<Instruction *> deps =
        ddgp.getDDG().getDependants(ddg_it->getValue());
    for (Instruction *I : deps) {
      PDG.addEdge(source, I);
    }
  }

  mdg_iterator mdg_begin = MDGTraits::nodes_begin(&mdgp.getMDG());
  mdg_iterator mdg_end = MDGTraits::nodes_end(&mdgp.getMDG());

  for (mdg_iterator mdg_it = mdg_begin; mdg_it != mdg_end; ++mdg_it) {
    Instruction *source = mdg_it->getValue();
    vector<Instruction *> deps =
        mdgp.getMDG().getDependants(mdg_it->getValue());
    for (Instruction *I : deps) {
      PDG.addEdge(source, I);
    }
  }

  // Add edges for phi nodes that have constant values; the edges start from the
  // last instruction of the BB that is associated with the constant value in
  // the phi node and they end in the phi node.
  for (ddg_iterator ddg_it = ddg_begin; ddg_it != ddg_end; ++ddg_it) {
    Instruction *target = ddg_it->getValue();
    if (PHINode *PN = dyn_cast<PHINode>(target)) {
      int nVals = PN->getNumIncomingValues();
      for (int i = 0; i < nVals; ++i) {
        if (dyn_cast<Constant>(PN->getIncomingValue(i))) {
          Instruction &source = PN->getIncomingBlock(i)->back();
          PDG.addEdge(&source, target);
        }
      }
    }
  }

  // Trim leaf jump instructions
  vector<PDGTraits::NodeType *> removeNodes;
  pdg_iterator pdg_begin = PDGTraits::nodes_begin(&PDG);
  pdg_iterator pdg_end = PDGTraits::nodes_end(&PDG);
  for (pdg_iterator pdg_it = pdg_begin; pdg_it != pdg_end; ++pdg_it) {
    Instruction *parent = pdg_it->getValue();
    for (ddg_child_iterator I = DDGTraits::child_begin(*pdg_it),
                            E = DDGTraits::child_end(*pdg_it);
         I != E; ++I) {
      DDGTraits::NodeType *target = *I;
      BranchInst *BI = dyn_cast<BranchInst>(target->getValue());
      if (BI != nullptr && BI->isUnconditional() &&
          target->getNumChildren() == 0) {
        removeNodes.push_back(target);
      }
    }
  }

  for (auto node : removeNodes) {
    PDG.removeNode(*node);
  }

  return false;
}
}
