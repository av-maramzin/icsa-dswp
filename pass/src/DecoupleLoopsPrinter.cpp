#include <iostream>
using std::cout;

#include <string>
using std::string;
#include <system_error>
using std::error_code;

#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/DebugInfoMetadata.h"
using llvm::DILocation;
using llvm::DIFile;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassRegistry.h"
using llvm::RegisterPass;

#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;

#include "PSG.h"
using icsa::PDGSCCGraphPass;

#include "DecoupleLoops.h"
using icsa::DecoupleLoopsPass;

namespace icsa {

struct DecoupleLoopsPrinter : public FunctionPass {
  static char ID;
  DecoupleLoopsPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    PDGSCCGraphPass &PSGP = Pass::getAnalysis<PDGSCCGraphPass>();
    DecoupleLoopsPass &DLP = Pass::getAnalysis<DecoupleLoopsPass>();

    const LoopInfo &LI = DLP.getLI();
    auto LoopToIterScc = DLP.getLoopToIterScc();
    auto LoopToWorkScc = DLP.getLoopToWorkScc();

    set<unsigned> lines;
    DIFile *File = nullptr;

    raw_os_ostream roos(cout);
    roos << "\nFunction " << F.getName() << ":\n\n";
    int count = 1;
    for (Loop *L : LI) {
      // Ignore loops we cannot decouple.
      if (LoopToWorkScc[L].size() == 0) continue;
      roos << "Loop " << count++ << ":\n";
      for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
           BI != BE; ++BI) {
        BasicBlock *BB = *BI;
        roos << '\n';
        BB->printAsOperand(roos);
        roos << '\n';
        for (Instruction &Inst : *BB) {
          if (DILocation *Loc = Inst.getDebugLoc()) {
            if (!File) {
              File = Loc->getFile();
            }
            lines.insert(Loc->getLine());
          }

          Inst.print(roos);
          if (LoopToWorkScc[L].find(PSGP.getSCC(&Inst)) !=
              LoopToWorkScc[L].end()) {
            roos << " ; work";
          }
          if (LoopToIterScc[L].find(PSGP.getSCC(&Inst)) !=
              LoopToIterScc[L].end()) {
            roos << " ; iter";
          }
          roos << '\n';
        }
      }
    }

    if (File != nullptr) {
      roos << " in file: ";
      File->print(roos);
      roos << '\n';
    }
    if (lines.size() > 0) {
      roos << " on lines:";
      for (unsigned line : lines) {
        roos << ' ' << line;
      }
      roos << '\n';
    }

    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<PDGSCCGraphPass>();
    AU.addRequired<DecoupleLoopsPass>();
  }
};

char DecoupleLoopsPrinter::ID = 0;
RegisterPass<DecoupleLoopsPrinter> DecoupleLoopsPrinterRegister(
    "print-decouple-loops", "Print the result of the -decouple-loops pass");
}
