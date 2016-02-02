#include "CDG.h"

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
#include "llvm/Analysis/PostDominators.h"
using llvm::PostDominatorTree;
#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;
#include "llvm/IR/Dominators.h"
using llvm::DomTreeNode;
#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/CFG.h"
using llvm::pred_iterator;
using llvm::pred_begin;
using llvm::pred_end;

#include "Util.h"

namespace icsa {

char ControlDependenceGraphPass::ID = 0;
RegisterPass<ControlDependenceGraphPass>
    CDGRegister("cdg", "Build Control Dependence Graph");

bool ControlDependenceGraphPass::runOnFunction(Function &F) {
  CDG.firstValue = &F.getEntryBlock();

  PostDominatorTree &pdt = Pass::getAnalysis<PostDominatorTree>();

  vector<DomTreeNode *> parents_stack;
  vector<DomTreeNode *> top_down_traversal;

  parents_stack.push_back(pdt.getRootNode());
  while (!parents_stack.empty()) {
    DomTreeNode *current = parents_stack.back();
    parents_stack.pop_back();
    // getBlock() might be null if there are multiple exits from F
    if (nullptr != current->getBlock()) {
      top_down_traversal.push_back(current);
    }
    for (DomTreeNode::iterator it = current->begin(); it != current->end();
         ++it) {
      parents_stack.push_back(*it);
    }
  }

  map<BasicBlock *, unique_ptr<set<BasicBlock *>>> dominance_frontier;

  // Traversing it in the opposite direction is bottom up.
  while (!top_down_traversal.empty()) {
    DomTreeNode *current = top_down_traversal.back();
    top_down_traversal.pop_back();

    BasicBlock *currentBB = current->getBlock();
    CDG.addNode(currentBB);

    // TODO(Stan): skip dominance_frontier and directly build CDG
    dominance_frontier[currentBB] =
        unique_ptr<set<BasicBlock *>>(new set<BasicBlock *>);

    for (pred_iterator it = pred_begin(currentBB); it != pred_end(currentBB);
         ++it) {
      if (pdt.getNode(*it)->getIDom() != current) {
        dominance_frontier[currentBB]->insert(*it);
      }
    }

    for (DomTreeNode::iterator it = current->begin(); it != current->end();
         ++it) {
      BasicBlock *childBlock = (*it)->getBlock(); // Z in the algorithm.
      for (BasicBlock *bb : *dominance_frontier[childBlock]) {
        if (pdt.getNode(bb)->getIDom() != current) {
          dominance_frontier[currentBB]->insert(bb);
        }
      }
    }
  }

  // Reverse the dominance_frontier map and store as a graph.
  for (auto &kv : dominance_frontier) {
    BasicBlock *to = kv.first;
    auto to_it = CDG.find(to);

    for (BasicBlock *from : *kv.second) {
      auto from_it = CDG.find(from);

      CDG.addEdge(from_it, to_it);
    }
  }

  return false;
}

}
