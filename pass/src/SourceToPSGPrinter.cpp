#include <string>
using std::string;
#include <system_error>
using std::error_code;

#include <fstream>
using std::ofstream;

#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/InstIterator.h"
using llvm::inst_iterator;
using llvm::inst_begin;
using llvm::inst_end;
#include "llvm/IR/DebugInfoMetadata.h"
using llvm::DILocation;
using llvm::DIFile;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassRegistry.h"
using llvm::RegisterPass;

#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "PSG.h"
using icsa::PDGSCCGraphPass;

#include "DependenceTraits.h"
using icsa::DepGraphTraitsWrapper;

#include "Util.h"
using icsa::instructionToFunctionName;

#include "InstSetDOTTraits.h"
// Defines DOTGraphTraits for DepGraphTraitsWrapper<Instruction>.

#include "GraphUtils.h"
using icsa::GraphUtils;

namespace icsa {

struct SourceToPSGPrinter : public FunctionPass {
  static char ID;
  SourceToPSGPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    PDGSCCGraphPass &psg = Pass::getAnalysis<PDGSCCGraphPass>();
    DependenceGraph<set<const Instruction *>> G = psg.getPSG();
    const set<const Instruction *> *firstSCC = G.nodes_begin()->first;
    string FuncName = instructionToFunctionName(**firstSCC->begin());
    string OutputFileName = FuncName + ".psg-report";

    auto ts = GraphUtils<set<const Instruction *>>::topsort(G);

    DIFile *File = nullptr; // The file in which this loop is defined.
    map <unsigned, set<unsigned>> lineToSCCs;
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
      auto &Inst = *I;
      DILocation *Loc = Inst.getDebugLoc();
      if (!Loc) {
        continue;
      }

      if (!File) {
        File = Loc->getFile();
      }
      int line = Loc->getLine();

      auto scc = psg.getSCC(&Inst);
      unsigned rank = find(ts.begin(), ts.end(), scc) - ts.begin();
      lineToSCCs[line].insert(rank);
    }

    if (File) {
      ofstream fout(OutputFileName);
      for (auto &line : lineToSCCs) {
        fout << File->getName().str() << ':' << line.first;
        for (auto &scc : line.second) {
          fout << ' ' << scc;
        }
        fout << '\n';
      }
      fout.close();
    }

    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<PDGSCCGraphPass>();
  }
};

char SourceToPSGPrinter::ID = 0;
RegisterPass<SourceToPSGPrinter>
    STPPRegister("print-source-to-psg", "Print the mapping from source code to PSG nodes");
}
