#ifndef ICSA_DSWP_INST_DOT_TRAITS_H
#define ICSA_DSWP_INST_DOT_TRAITS_H

#include <string>
using std::string;
#include <sstream>
using std::stringstream;

#include "llvm/IR/Instruction.h"
using llvm::Instruction;

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
struct DOTGraphTraits<DepGraphTraitsWrapper<set<const Instruction *>>>
    : public DefaultDOTGraphTraits {
  typedef DepGraphTraitsWrapper<set<const Instruction *>> GraphType;
  typedef DepNodeTraitsWrapper<set<const Instruction *>> NodeType;

  typedef DOTGraphTraits<const Function *> func_traits;
  func_traits FT;

  DOTGraphTraits(bool isSimple = false)
      : FT(isSimple), DefaultDOTGraphTraits(isSimple){};

  static string getGraphName(const GraphType &Graph) {
    string FuncName =
        instructionToFunctionName(**Graph.nodes_begin()->getValue()->begin());
    return "Dependence graph for '" + FuncName + "' function";
  }

  string
  getNodeLabel(const DepNodeTraitsWrapper<set<const Instruction *>> *Node,
               const DepGraphTraitsWrapper<set<const Instruction *>> &Graph) {
    stringstream ss;
    raw_os_ostream roos(ss);

    for (const Instruction *Inst : *Node->getValue()) {
      Inst->print(roos);
      roos << '\n';
    }

    return ss.str();
  }
};
}

#endif
