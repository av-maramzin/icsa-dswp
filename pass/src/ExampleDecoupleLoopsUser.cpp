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

struct ExampleDecoupleLoopsUser : public FunctionPass {
  static char ID;
  ExampleDecoupleLoopsUser() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    DecoupleLoopsPass &DLP = Pass::getAnalysis<DecoupleLoopsPass>();

    const LoopInfo *LI = DLP.getLI(&F);

    raw_os_ostream roos(cout);
    for (Loop *L : *LI) {
      // Ignore loops we cannot decouple.
      if (!DLP.hasWork(L)) continue;
      for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
           BI != BE; ++BI) {
        BasicBlock *BB = *BI;
        for (Instruction &Inst : *BB) {
          if (DLP.isWork(Inst, L)) {
            // Inst, from loop L, is a work instruction
          }
          if (DLP.isIter(Inst, L)) {
            // Inst, from loop L, is an iter instruction
          }
        }
      }
    }

    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<DecoupleLoopsPass>();
  }
};

char ExampleDecoupleLoopsUser::ID = 0;
RegisterPass<ExampleDecoupleLoopsUser> EDLURegister(
    "example-decouple-loops-user", "Example user of the -decouple-loops pass");
}
