#ifndef ICSA_DSWP_BASIC_BLOCK_DOT_TRAITS_H
#define ICSA_DSWP_BASIC_BLOCK_DOT_TRAITS_H

#include <string>
using std::string;
#include <sstream>
using std::stringstream;

#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;

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
using icsa::basicBlockToFunctionName;

namespace llvm {

template <>
struct DOTGraphTraits<DepGraphTraitsWrapper<BasicBlock>>
    : public DefaultDOTGraphTraits {
  typedef DepGraphTraitsWrapper<BasicBlock> GraphType;
  typedef DepNodeTraitsWrapper<BasicBlock> NodeType;

  typedef DOTGraphTraits<const Function *> func_traits;
  func_traits FT;

  DOTGraphTraits(bool isSimple = false)
      : FT(isSimple), DefaultDOTGraphTraits(isSimple){};

  static string getGraphName(const GraphType &Graph) {
    string FuncName =
        basicBlockToFunctionName(*Graph.nodes_begin()->getValue());
    return "Dependence graph for '" + FuncName + "' function";
  }

  string getNodeLabel(const DepNodeTraitsWrapper<BasicBlock> *Node,
                      const DepGraphTraitsWrapper<BasicBlock> &Graph) {
    return FT.getNodeLabel(Node->getValue(), Node->getValue()->getParent());
  }
};
}

#endif
