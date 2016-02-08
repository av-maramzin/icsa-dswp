#ifndef ICSA_DSWP_PSG_PRINTER_H
#define ICSA_DSWP_PSG_PRINTER_H

#include <string>
using std::string;
#include <sstream>
using std::stringstream;

#include "llvm/Support/DOTGraphTraits.h"
using llvm::DOTGraphTraits;
#include "llvm/Analysis/CFGPrinter.h"
// Specializes DOTGraphTraits for <Function *>

#include "PdgSccGraph.h"

#include "Util.h"

namespace llvm {

template <>
struct DOTGraphTraits<const icsa::PdgSccGraph *>
    : public DefaultDOTGraphTraits {

  typedef DOTGraphTraits<const Function *> func_traits;
  func_traits FT;

  DOTGraphTraits(bool isSimple = false)
      : FT(isSimple), DefaultDOTGraphTraits(isSimple){};

  static string getGraphName(const icsa::PdgSccGraph *PDG) {
    return "PDG for '" + PDG->getFunction()->getName().str() + "' function";
  }

  string getNodeLabel(const icsa::PdgSccNode *Node,
                      const icsa::PdgSccGraph *Graph) {
    stringstream ss;
    raw_os_ostream roos(ss);

    for (auto Inst : *Node->getValue()) {
      Inst->print(roos);
      roos << '\n';
    }

    return ss.str();
  }
};
}

#endif
