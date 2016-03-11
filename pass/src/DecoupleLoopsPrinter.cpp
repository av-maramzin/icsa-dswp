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

struct DecoupleLoopsPrinter : public FunctionPass {
  static char ID;
  DecoupleLoopsPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    DecoupleLoopsPass &DLP = Pass::getAnalysis<DecoupleLoopsPass>();

    const LoopInfo &LI = DLP.getLI(&F);

    raw_os_ostream roos(cout);
    roos << "\nFunction " << F.getName() << ":\n\n";
    int count = 1;
    for (Loop *L : LI) {
      set<unsigned> lines;
      DIFile *File = nullptr;

      // Ignore loops we cannot decouple.
      if (!DLP.hasWork(L)) continue;
      // Ignore loops that are not top-level (nested loops).
      if (L->getLoopDepth() > 1) continue;
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
          if (DLP.isWork(Inst, L)) {
            roos << " ; work";
          }
          if (DLP.isIter(Inst, L)) {
            roos << " ; iter";
          }
          roos << '\n';
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
    }

    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<DecoupleLoopsPass>();
  }
};

char DecoupleLoopsPrinter::ID = 0;
RegisterPass<DecoupleLoopsPrinter> DecoupleLoopsPrinterRegister(
    "print-decouple-loops", "Print the result of the -decouple-loops pass");
}
