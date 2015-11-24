#include "CDG.h"

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
using llvm::succ_iterator;
using llvm::succ_begin;
using llvm::succ_end;
#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;

#include <iostream>
using std::cout;

#include <map>
using std::map;
#include <vector>
using std::vector;
#include <set>
using std::set;

#include "Util.h"

char ControlDependenceGraph::ID = 0;
RegisterPass<ControlDependenceGraph>
    CDGRegister("cdg", "Build Control Dependence Graph");

ControlDependenceGraph::ControlDependenceGraph() : FunctionPass(ID) {}

void ControlDependenceGraph::getAnalysisUsage(AnalysisUsage &Info) const {
  Info.addRequired<PostDominatorTree>();
}

bool ControlDependenceGraph::runOnFunction(Function &F) {
  PostDominatorTree &pdt = Pass::getAnalysis<PostDominatorTree>();

  vector<DomTreeNode *> parents_stack;
  vector<DomTreeNode *> top_down_traversal;

  parents_stack.push_back(pdt.getRootNode());
  while (!parents_stack.empty()) {
    DomTreeNode *current = parents_stack.back();
    parents_stack.pop_back();
    top_down_traversal.push_back(current);
    for (DomTreeNode::iterator it = current->begin(); it != current->end();
         ++it) {
      parents_stack.push_back(*it);
    }
  }

  LOG_DEBUG("Top-down traversal of post-dominator tree:");
  for (DomTreeNode *node : top_down_traversal) {
    raw_os_ostream roos(cout);
    node->getBlock()->print(roos);
  }

  map<BasicBlock *, set<BasicBlock *> *> dominance_frontier;

  // Traversing it in the opposite direction would be bottom up.
  while (!top_down_traversal.empty()) {
    DomTreeNode *current = top_down_traversal.back();
    top_down_traversal.pop_back();

    BasicBlock *currentBB = current->getBlock();

    dominance_frontier.insert({currentBB, new set<BasicBlock *>()});

    for (succ_iterator it = succ_begin(currentBB); it != succ_end(currentBB);
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

  LOG_DEBUG("Dominance frontier:");
  for (auto &kv : dominance_frontier) {
    raw_os_ostream roos(cout);
    kv.first->print(roos);
    LOG_DEBUG("has frontier");
    for (auto &v : *kv.second) {
      v->print(roos);
    }
    LOG_DEBUG("=========");
  }

  // Free heap memory.
  for (auto &kv : dominance_frontier) {
    delete kv.second;
  }

  LOG_INFO("Finished.");
  return true;
}
