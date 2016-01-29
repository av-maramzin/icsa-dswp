#ifndef ICSA_DSWP_PDG_PRINTER_H
#define ICSA_DSWP_PDG_PRINTER_H

#include <string>
using std::string;
#include <sstream>
using std::stringstream;

#include "llvm/Support/DOTGraphTraits.h"
using llvm::DOTGraphTraits;
#include "llvm/Analysis/CFGPrinter.h"
// Specializes DOTGraphTraits for <Function *>

#include "PDG.h"

#include "Util.h"

namespace llvm {

template <>
struct DOTGraphTraits<const icsa::ProgramDependenceGraph *>
    : public DefaultDOTGraphTraits {

  typedef DOTGraphTraits<const Function *> func_traits;
  func_traits FT;

  DOTGraphTraits(bool isSimple = false)
      : FT(isSimple), DefaultDOTGraphTraits(isSimple){};

  static string getGraphName(const icsa::ProgramDependenceGraph *PDG) {
    return "PDG for '" + PDG->getFunction()->getName().str() + "' function";
  }

  string getNodeLabel(const icsa::ProgramDependenceNode *Node,
                      const icsa::ProgramDependenceGraph *Graph) {
    stringstream ss;
    raw_os_ostream roos(ss);

    Node->getValue()->print(roos);

    return ss.str();
  }
};
}

#endif
