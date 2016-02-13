#include "CDG.h"

#include <memory>
using std::unique_ptr;
#include <map>
using std::map;
#include <vector>
using std::vector;
#include <stack>
using std::stack;
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

// Depth-first search the post-dominator tree in order to construct a top-down
// traversal.
stack<DomTreeNode *> getBottomUpTraversal(const PostDominatorTree &pdt) {
  stack<DomTreeNode *> parents;
  stack<DomTreeNode *> result;

  parents.push(pdt.getRootNode());
  while (!parents.empty()) {
    DomTreeNode *current = parents.top();
    parents.pop();
    // getBlock() might be null if there are multiple exits from F
    if (nullptr != current->getBlock()) {
      result.push(current);
    }
    for (DomTreeNode::iterator it = current->begin(); it != current->end();
         ++it) {
      parents.push(*it);
    }
  }

  return result;
}

bool ControlDependenceGraphPass::runOnFunction(Function &F) {
  PostDominatorTree &pdt = Pass::getAnalysis<PostDominatorTree>();

  stack<DomTreeNode *> bottom_up_traversal = getBottomUpTraversal(pdt);

  map<BasicBlock *, set<BasicBlock *>> post_dom_frontier;

  while (!bottom_up_traversal.empty()) {
    DomTreeNode *current = bottom_up_traversal.top();
    // This returns `void`, unfortunately.
    bottom_up_traversal.pop();

    BasicBlock *currentBB = current->getBlock();
    CDG.addNode(currentBB);

    // TODO(Stan): skip post_dom_frontier and directly build CDG
    // Construct an empty frontier set for `currentBB`.
    post_dom_frontier[currentBB];

    // For each predecessor of `currentBB` in the CFG.
    for (pred_iterator it = pred_begin(currentBB); it != pred_end(currentBB);
         ++it) {
      // If it is not immediately post-dominated by `currentBB`, then it is in
      // the post-dominance frontier of `currentBB`.
      if (pdt.getNode(*it)->getIDom() != current) {
        post_dom_frontier[currentBB].insert(*it);
      }
    }

    // For each node post-dominated by `current` (i.e. for each 'child' node of
    // `current`).
    for (DomTreeNode::iterator it = current->begin(); it != current->end();
         ++it) {
      BasicBlock *childBlock = (*it)->getBlock(); // Z in the algorithm.
      // For each block in the post_dom_frontier of the 'child' node.
      for (BasicBlock *bb : post_dom_frontier[childBlock]) {
        // Same as before.
        if (pdt.getNode(bb)->getIDom() != current) {
          post_dom_frontier[currentBB].insert(bb);
        }
      }
    }
  }

  // Reverse the post_dom_frontier map and store as a graph.
  for (auto &kv : post_dom_frontier) {
    BasicBlock *to = kv.first;
    for (BasicBlock *from : kv.second) {
      CDG.addEdge(from, to);
    }
  }

  return false;
}
}
