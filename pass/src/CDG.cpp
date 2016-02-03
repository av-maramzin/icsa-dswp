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

// Get the post-dominator frontier given a post-dominator tree and a bottom-up
// traversal for it.
map<BasicBlock *, set<BasicBlock *>>
getPostDomFrontier(const PostDominatorTree &pdt,
                   stack<DomTreeNode *> &&bottom_up_traversal) {
  map<BasicBlock *, set<BasicBlock *>> result;

  while (!bottom_up_traversal.empty()) {
    DomTreeNode *current = bottom_up_traversal.top();
    // This returns `void`, unfortunately.
    bottom_up_traversal.pop();

    BasicBlock *currentBB = current->getBlock();

    // Construct an empty frontier set for `currentBB`.
    result[currentBB];

    // For each predecessor of `currentBB` in the CFG.
    for (pred_iterator it = pred_begin(currentBB); it != pred_end(currentBB);
         ++it) {
      // If it is not immediately post-dominated by `currentBB`, then it is in
      // the post-dominance frontier of `currentBB`.
      if (pdt.getNode(*it)->getIDom() != current) {
        result[currentBB].insert(*it);
      }
    }

    // For each node post-dominated by `current` (i.e. for each 'child' node of
    // `current`).
    for (DomTreeNode::iterator it = current->begin(); it != current->end();
         ++it) {
      BasicBlock *childBlock = (*it)->getBlock(); // Z in the algorithm.
      // For each block in the result of the 'child' node.
      for (BasicBlock *bb : result[childBlock]) {
        // Same as before.
        if (pdt.getNode(bb)->getIDom() != current) {
          result[currentBB].insert(bb);
        }
      }
    }
  }

  return result;
}

bool ControlDependenceGraphPass::runOnFunction(Function &F) {
  const PostDominatorTree &pdt = Pass::getAnalysis<PostDominatorTree>();

  stack<DomTreeNode *> bottom_up_traversal = getBottomUpTraversal(pdt);

  map<BasicBlock *, set<BasicBlock *>> post_dom_frontier =
      getPostDomFrontier(pdt, std::move(bottom_up_traversal));

  // Reverse the post_dom_frontier map and store as a graph.
  for (auto &kv : post_dom_frontier) {
    BasicBlock *node = kv.first;
    CDG.addNode(node);
  }
  for (auto &kv : post_dom_frontier) {
    BasicBlock *to = kv.first;
    for (BasicBlock *from : kv.second) {
      CDG.addEdge(from, to);
    }
  }

  return false;
}
}
