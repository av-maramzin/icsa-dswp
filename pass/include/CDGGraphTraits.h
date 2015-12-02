// Used in CDGPrinter.cpp.

#include "CDG.h"
using icsa::ControlDependenceNode;
using icsa::ControlDependenceGraph;
using icsa::cdg_iterator;
using icsa::cdg_const_iterator;

#include "llvm/ADT/GraphTraits.h"
#include "llvm/IR/CFG.h"
// Defines GraphTraits for <Function *> and <BasicBlock *>.
#include "llvm/Support/DOTGraphTraits.h"
#include "llvm/Analysis/CFGPrinter.h"
// Defines DOTGraphTraits for <Function *>.

#include "Util.h"
using icsa::getBBSourceCode;

namespace llvm {

template <> struct GraphTraits<ControlDependenceNode *> {
  typedef ControlDependenceNode NodeType;
  typedef ControlDependenceNode::iterator ChildIteratorType;

  static NodeType *getEntryNode(ControlDependenceNode *CDN) { return CDN; }
  static inline ChildIteratorType child_begin(NodeType *N) {
    return N->begin();
  }
  static inline ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <> struct GraphTraits<const ControlDependenceNode *> {
  typedef const ControlDependenceNode NodeType;
  typedef ControlDependenceNode::const_iterator ChildIteratorType;

  static NodeType *getEntryNode(const ControlDependenceNode *CDN) {
    return CDN;
  }

  static inline ChildIteratorType child_begin(NodeType *N) {
    return N->begin();
  }

  static inline ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <>
struct GraphTraits<ControlDependenceGraph *>
    : public GraphTraits<ControlDependenceNode *> {
  static NodeType *getEntryNode(ControlDependenceGraph *CDG) {
    return CDG->getRootNode();
  }

  typedef cdg_iterator nodes_iterator;

  static nodes_iterator nodes_begin(ControlDependenceGraph *CDG) {
    return cdg_iterator(*CDG);
  }
  static nodes_iterator nodes_end(ControlDependenceGraph *CDG) {
    return cdg_iterator(*CDG, true);
  }
  static size_t size(ControlDependenceGraph *CDG) { return CDG->getNumNodes(); }
};

template <>
struct GraphTraits<const ControlDependenceGraph *>
    : public GraphTraits<const ControlDependenceNode *> {
  static NodeType *getEntryNode(const ControlDependenceGraph *CDG) {
    return CDG->getRootNode();
  }

  typedef cdg_const_iterator nodes_iterator;

  static nodes_iterator nodes_begin(const ControlDependenceGraph *CDG) {
    return cdg_const_iterator(*CDG);
  }
  static nodes_iterator nodes_end(const ControlDependenceGraph *CDG) {
    return cdg_const_iterator(*CDG, true);
  }
  static size_t size(const ControlDependenceGraph *CDG) {
    return CDG->getNumNodes();
  }
};
template <>
struct DOTGraphTraits<const ControlDependenceGraph *>
    : public DefaultDOTGraphTraits {

  typedef DOTGraphTraits<const Function *> func_traits;
  func_traits FT;

  DOTGraphTraits(bool isSimple = false)
      : FT(isSimple), DefaultDOTGraphTraits(isSimple){};

  static string getGraphName(const ControlDependenceGraph *CDG) {
    return "CDG for '" + CDG->getFunction()->getName().str() + "' function";
  }

  static string getSimpleNodeLabel(const ControlDependenceNode *Node,
                                   const ControlDependenceGraph *Graph) {
    return func_traits::getSimpleNodeLabel(Node->getBlock(), Graph->getFunction());
  }

  static string getCompleteNodeLabel(const ControlDependenceNode *Node,
        const ControlDependenceGraph *Graph) {
    return getBBSourceCode(*Node->getBlock());
  }

  string getNodeLabel(const ControlDependenceNode *Node,
                      const ControlDependenceGraph *Graph) {
    if (isSimple()) {
      return getSimpleNodeLabel(Node, Graph);
    } else {
      return getCompleteNodeLabel(Node, Graph);
    }
  }
};
}
