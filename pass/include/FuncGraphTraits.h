#ifndef ICSA_DSWP_CFG_PRINTER_H
#define ICSA_DSWP_CFG_PRINTER_H

#include <string>
using std::string;

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
using llvm::Function;

#include "llvm/ADT/GraphTraits.h"
#include "llvm/IR/CFG.h"
// Defines GraphTraits for <Function *> and <BasicBlock *>.
#include "llvm/Support/DOTGraphTraits.h"
#include "llvm/Analysis/CFGPrinter.h"
// Defines DOTGraphTraits for <Function *>.

#include "Util.h"
using icsa::getBBSourceCode;

namespace icsa {
struct PrintFunction {
  Function *F;
};
}
using icsa::PrintFunction;

namespace llvm {

template <>
struct GraphTraits<PrintFunction *> : public GraphTraits<Function *> {
  typedef GraphTraits<Function *> super;

  static NodeType *getEntryNode(PrintFunction *PF) {
    return super::getEntryNode(PF->F);
  }

  static nodes_iterator nodes_begin(PrintFunction *PF) {
    return super::nodes_begin(PF->F);
  }

  static nodes_iterator nodes_end(PrintFunction *PF) {
    return super::nodes_end(PF->F);
  }

  static size_t size(PrintFunction *PF) { return super::size(PF->F); }
};

template <>
struct GraphTraits<const PrintFunction *>
    : public GraphTraits<const Function *> {
  typedef GraphTraits<const Function *> super;

  static NodeType *getEntryNode(const PrintFunction *PF) {
    return super::getEntryNode(PF->F);
  }

  static nodes_iterator nodes_begin(const PrintFunction *PF) {
    return super::nodes_begin(PF->F);
  }

  static nodes_iterator nodes_end(const PrintFunction *PF) {
    return super::nodes_end(PF->F);
  }

  static size_t size(const PrintFunction *PF) { return super::size(PF->F); }
};

template <>
struct DOTGraphTraits<const PrintFunction *> : public DefaultDOTGraphTraits {

  typedef DOTGraphTraits<const Function *> func_traits;
  func_traits FT;

  DOTGraphTraits(bool isSimple = false)
      : FT(isSimple), DefaultDOTGraphTraits(isSimple){};

  static string getGraphName(const PrintFunction *PF) {
    return func_traits::getGraphName(PF->F);
  }

  static string getSimpleNodeLabel(const BasicBlock *Node,
                                   const PrintFunction *PF) {
    return func_traits::getSimpleNodeLabel(Node, PF->F);
  }

  static string getCompleteNodeLabel(const BasicBlock *Node,
                                     const PrintFunction *PF) {
    return getBBSourceCode(*Node);
  }

  string getNodeLabel(const BasicBlock *Node, const PrintFunction *PF) {
    if (isSimple()) {
      return getSimpleNodeLabel(Node, PF);
    } else {
      return getCompleteNodeLabel(Node, PF);
    }
  }
};
}

#endif
