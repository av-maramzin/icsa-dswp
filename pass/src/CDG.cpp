#include "CDG.h"

#include <iostream>
using std::cout;

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
#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;
#include "llvm/ADT/STLExtras.h"
using llvm::make_unique;

#include "Util.h"

char ControlDependenceGraph::ID = 0;
RegisterPass<ControlDependenceGraph>
    CDGRegister("cdg", "Build Control Dependence Graph");

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

  map<BasicBlock *, set<BasicBlock *> *> dominance_frontier;


  // Traversing it in the opposite direction is bottom up.
  while (!top_down_traversal.empty()) {
    DomTreeNode *current = top_down_traversal.back();
    top_down_traversal.pop_back();

    BasicBlock *currentBB = current->getBlock();

    // TODO(Stan): skip dominance_frontier and directly build CDG
    dominance_frontier.insert({currentBB, new set<BasicBlock *>()});

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
    CDGNodeMapType::iterator to_it = CDGNodes.find(to);
    if (to_it == CDGNodes.end())
      to_it = CDGNodes.insert(
          CDGNodes.end(),
          make_pair(to, make_unique<ControlDependenceNode>(to)));

    for (BasicBlock *from : *kv.second) {
      CDGNodeMapType::iterator from_it = CDGNodes.find(from);
      if (from_it == CDGNodes.end())
        from_it = CDGNodes.insert(
            CDGNodes.end(),
            make_pair(from, make_unique<ControlDependenceNode>(from)));

      auto from_node = from_it->second.get();
      auto to_node = move(to_it->second);
      to_node = from_node->addChild(move(to_node));
      CDGNodes[to] = move(to_node);
    }
  }

  raw_os_ostream roos(cout);
  pdt.print(roos, nullptr);
  print(roos, nullptr);

  // Free heap memory.
  for (auto &kv : dominance_frontier) {
    delete kv.second;
  }

  return true;
}

ControlDependenceNode *ControlDependenceGraph::
operator[](BasicBlock *BB) const {
  return getNode(BB);
}

ControlDependenceNode *ControlDependenceGraph::getNode(BasicBlock *BB) const {
  CDGNodeMapType::const_iterator I = CDGNodes.find(BB);
  if (I != CDGNodes.end())
    return I->second.get();
  return nullptr;
}

bool ControlDependenceGraph::dependsOn(ControlDependenceNode *A,
                                       ControlDependenceNode *B) const {
  for (ControlDependenceNode::const_iterator it = B->begin(); it != B->end();
       ++it) {
    if (false == (*it)->compare(A))
      return true;
  }

  for (ControlDependenceNode::const_iterator it = B->begin(); it != B->end();
       ++it) {
    if (dependsOn(A, (*it)))
      return true;
  }

  return false;
}

bool ControlDependenceGraph::dependsOn(BasicBlock *A, BasicBlock *B) const {
  ControlDependenceNode *AN = getNode(A), *BN = getNode(B);
  return dependsOn(AN, BN);
}

void ControlDependenceGraph::getDependants(
    BasicBlock *R, SmallVectorImpl<BasicBlock *> &Result) const {
  Result.clear();
  const ControlDependenceNode *RN = getNode(R);
  for (ControlDependenceNode::const_iterator it = RN->begin(); it != RN->end();
       ++it) {
    Result.push_back((*it)->getBlock());
  }
}

void ControlDependenceGraph::releaseMemory() { CDGNodes.clear(); }

void ControlDependenceGraph::print(raw_ostream &OS, const Module *) const {
  OS << "=============================--------------------------------\n";
  OS << "Control Dependence Graph: ";
  OS << "<node: dependants>";
  OS << "\n";
  for (CDGNodeMapType::const_iterator I = CDGNodes.begin(); I != CDGNodes.end();
       ++I) {
    I->second.get()->print(OS);
    OS << '\n';
  }
}
