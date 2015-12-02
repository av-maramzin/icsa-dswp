#include "CDG.h"

#include <iostream>
using std::cout;
#include <memory>
using std::unique_ptr;
#include <utility>
using std::move;

#include <vector>
using std::vector;
#include <map>
using std::map;
#include <set>
using std::set;

#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;

#include "llvm/PassSupport.h"
using llvm::RegisterPass;

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

#include "llvm/Analysis/PostDominators.h"
using llvm::PostDominatorTree;

#include "Util.h"

namespace icsa {

char ControlDependenceGraph::ID = 0;
RegisterPass<ControlDependenceGraph>
    CDGRegister("cdg", "Build Control Dependence Graph");

bool ControlDependenceGraph::runOnFunction(Function &F) {
  firstValue = &F.getEntryBlock();

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
    Nodes[currentBB] =
        shared_ptr<ControlDependenceNode>(new ControlDependenceNode(currentBB));

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
    NodeMapType::iterator to_it = Nodes.find(to);

    for (BasicBlock *from : *kv.second) {
      NodeMapType::iterator from_it = Nodes.find(from);

      auto from_node = from_it->second.get();
      auto to_node = to_it->second;
      from_node->addChild(to_node);
    }
  }

  return true;
}

void ControlDependenceGraph::releaseMemory() {
  firstValue = nullptr;
  Nodes.clear();
}

void ControlDependenceGraph::print(raw_ostream &OS, const Module *) const {
  OS << "=============================--------------------------------\n";
  OS << "Control Dependence Graph: ";
  OS << "<node: dependants>";
  OS << "\n";
  for (NodeMapType::const_iterator I = Nodes.begin(); I != Nodes.end();
       ++I) {
    I->second.get()->print(OS);
    OS << '\n';
  }
}
}
