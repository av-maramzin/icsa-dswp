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

#include "DecoupleLoops.h"
using icsa::DecoupleLoopsPass;

namespace icsa {

struct DecoupleSourceSplitPrinter : public FunctionPass {
  static char ID;
  DecoupleSourceSplitPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    DecoupleLoopsPass &DLP = Pass::getAnalysis<DecoupleLoopsPass>();

    const LoopInfo &LI = DLP.getLI(&F);

    DIFile *File = nullptr; // The file in which this loop is defined.

    raw_os_ostream roos(cout);
    for (Loop *L : LI) {
      // Ignore loops we cannot decouple.
      if (!DLP.hasWork(L))
        continue;

      unsigned firstLine = 0;
      unsigned lastLine = 0;
      for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
           BI != BE; ++BI) {
        BasicBlock *BB = *BI;
        for (Instruction &Inst : *BB) {
          DILocation *Loc = Inst.getDebugLoc();
          if (!Loc) {
            continue;
          }
          if (!File) {
            File = Loc->getFile();
          }
          int line = Loc->getLine();
          if (firstLine == 0)
            firstLine = line;
          if (line > lastLine)
            lastLine = line;
        }
      }

      roos << File->getFilename() << " " << firstLine << " " << lastLine
           << "\n";
    }

    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<DecoupleLoopsPass>();
  }
};

char DecoupleSourceSplitPrinter::ID = 0;
RegisterPass<DecoupleSourceSplitPrinter>
    DSSPRegister("print-decouple-source-split",
                 "Print beginnings and ends of loops that can be decoupled");
}
