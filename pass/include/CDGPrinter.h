#ifndef ICSA_DSWP_CDG_PRINTER_H
#define ICSA_DSWP_CDG_PRINTER_H

#include <string>
using std::string;

#include "llvm/Support/DOTGraphTraits.h"
using llvm::DOTGraphTraits;
#include "llvm/Analysis/CFGPrinter.h"
// Specializes DOTGraphTraits for <Function *>

#include "CDG.h"

#include "Util.h"

namespace llvm {

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

#endif
