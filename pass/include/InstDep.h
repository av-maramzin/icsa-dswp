#ifndef ICSA_DSWP_INST_DEP_H
#define ICSA_DSWP_INST_DEP_H

#include "llvm/ADT/GraphTraits.h"

#include "DependenceGraphTraits.h"

#include "llvm/IR/Instruction.h"
using llvm::Instruction;

namespace icsa {

typedef DependenceNode<Instruction> InstructionDependenceNode;

}

namespace llvm {

template <>
struct GraphTraits<icsa::InstructionDependenceNode *>
    : public icsa::DNGraphTraits<Instruction> {};

template <>
struct GraphTraits<const icsa::InstructionDependenceNode *>
    : public icsa::ConstDNGraphTraits<Instruction> {};

}

#endif
