#ifndef ICSA_DSWP_CDG_PRINTER_H
#define ICSA_DSWP_CDG_PRINTER_H

#include <string>
using std::string;

#include "llvm/Support/DOTGraphTraits.h"
using llvm::DOTGraphTraits;
#include "llvm/Analysis/CFGPrinter.h"
// Specializes DOTGraphTraits for <Function *>

#include "CDG.h"

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

  string getNodeLabel(const ControlDependenceNode *Node,
                      const ControlDependenceGraph *CDG) {
    return FT.getNodeLabel(Node->getBlock(), CDG->getFunction());
  }
};

#endif
