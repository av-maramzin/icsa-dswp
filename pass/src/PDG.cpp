#include "PDG.h"

#include <vector>
using std::vector;

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

#include "Dependence.h"
using icsa::DependenceGraph;

#include "CDG.h"
using icsa::ControlDependenceGraphPass;
#include "MDG.h"
using icsa::MemoryDependenceGraphPass;
#include "DDG.h"
using icsa::DataDependenceGraphPass;

namespace icsa {

char ProgramDependenceGraphPass::ID = 0;
RegisterPass<ProgramDependenceGraphPass>
    PDGRegister("pdg", "Build Program Dependence Graph");

bool ProgramDependenceGraphPass::runOnFunction(Function &F) {
  const DependenceGraph<BasicBlock> &cdg =
      Pass::getAnalysis<ControlDependenceGraphPass>().getCDG();
  const DependenceGraph<Instruction> &mdg =
      Pass::getAnalysis<MemoryDependenceGraphPass>().getMDG();
  const DependenceGraph<Instruction> &ddg =
      Pass::getAnalysis<DataDependenceGraphPass>().getDDG();

  // All nodes in the data dependence graph are nodes of the program dependence
  // graph.
  for (auto I = ddg.nodes_cbegin(), E = ddg.nodes_cend(); I != E; ++I) {
    PDG.addNode(I->first);
  }

  // Copy the DDG edges to the PDG.
  for (auto I = ddg.nodes_cbegin(), E = ddg.nodes_cend(); I != E; ++I) {
    const Instruction *source = I->first;
    for (const Instruction *target : I->second) {
      PDG.addEdge(source, target);
    }
  }

  // Copy the MDG edges to the PDG>
  for (auto I = mdg.nodes_cbegin(), E = mdg.nodes_cend(); I != E; ++I) {
    const Instruction *source = I->first;
    for (const Instruction *target : I->second) {
      PDG.addEdge(source, target);
    }
  }

  // For each dependence in the CDG: construct a dependence from the last
  // instruction of the source basic block to the each instruction in the
  // target basic block.
  for (auto I = cdg.nodes_cbegin(), E = cdg.nodes_cend(); I != E; ++I) {
    const Instruction &source = I->first->back();
    for (const BasicBlock *BB : I->second) {
      for (BasicBlock::const_iterator it = BB->begin(); it != BB->end(); ++it) {
        PDG.addEdge(&source, &*it);
      }
    }
  }

  // Add edges for phi nodes that have constant values; the edges start from the
  // last instruction of the BB that is associated with the constant value in
  // the phi node and they end in the phi node.
  for (auto I = ddg.nodes_cbegin(), E = ddg.nodes_cend(); I != E; ++I) {
    const Instruction *target = I->first;
    if (const PHINode *PN = dyn_cast<PHINode>(target)) {
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
  vector<const Instruction *> removeNodes;
  for (auto I = PDG.nodes_cbegin(), E = PDG.nodes_cend(); I != E; ++I) {
    const BranchInst *BI = dyn_cast<BranchInst>(I->first);
    if (BI != nullptr && BI->isUnconditional() && I->second.size() == 0) {
      removeNodes.push_back(I->first);
    }
  }
  for (auto node : removeNodes) {
    PDG.removeNode(node);
  }

  return false;
}
}
