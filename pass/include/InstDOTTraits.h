#ifndef ICSA_DSWP_INST_DOT_TRAITS_H
#define ICSA_DSWP_INST_DOT_TRAITS_H

#include <string>
using std::string;
#include <sstream>
using std::stringstream;

#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;

#include "llvm/Support/DOTGraphTraits.h"
using llvm::DOTGraphTraits;
#include "llvm/Analysis/CFGPrinter.h"
// Specializes DOTGraphTraits for <Function *>

#include "DependenceTraits.h"
using icsa::DepGraphTraitsWrapper;
using icsa::DepNodeTraitsWrapper;

#include "Util.h"
using icsa::instructionToFunctionName;

namespace llvm {

template <>
struct DOTGraphTraits<DepGraphTraitsWrapper<Instruction>>
    : public DefaultDOTGraphTraits {
  typedef DepGraphTraitsWrapper<Instruction> GraphType;
  typedef DepNodeTraitsWrapper<Instruction> NodeType;

  typedef DOTGraphTraits<const Function *> func_traits;
  func_traits FT;

  DOTGraphTraits(bool isSimple = false)
      : FT(isSimple), DefaultDOTGraphTraits(isSimple){};

  static string getGraphName(const GraphType &Graph) {
    string FuncName =
        instructionToFunctionName(*Graph.nodes_begin()->getValue());
    return "Dependence graph for '" + FuncName + "' function";
  }

  string getNodeLabel(const DepNodeTraitsWrapper<Instruction> *Node,
                      const DepGraphTraitsWrapper<Instruction> &Graph) {
    stringstream ss;
    raw_os_ostream roos(ss);

    Node->getValue()->print(roos);

    return ss.str();
  }
};
}

#endif
