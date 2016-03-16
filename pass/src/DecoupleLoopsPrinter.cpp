#include <iostream>
using std::cout;

#include <set>
using std::set;

#include <vector>
using std::vector;

#include <string>
using std::string;
#include <system_error>
using std::error_code;

#include "llvm/IR/Instructions.h"
using llvm::BranchInst;
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

#include "llvm/Support/Casting.h"
using llvm::dyn_cast;

#include "DecoupleLoops.h"
using icsa::DecoupleLoopsPass;

namespace icsa {

void printLocInfo(DIFile *File, const set<unsigned> &lines, raw_os_ostream &roos);

struct DecoupleLoopsPrinter : public FunctionPass {
  static char ID;
  DecoupleLoopsPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    DecoupleLoopsPass &DLP = Pass::getAnalysis<DecoupleLoopsPass>();

    const LoopInfo &LI = DLP.getLI(&F);

    DIFile *File = nullptr; // The file in which this loop is defined.
    vector<int> lines;      // Line per instruction in the order of traversal.
    vector<int> cols;       // Column per instruction in the order of traversal.
    vector<int> BBs;        // Basic-block for each line.
    vector<int> Loops;        // Loop for each line.
    vector<bool> branchLines; // true if instruction n is a branch instruction
    vector<bool> workLines;
    vector<bool> iterLines;

    int bb_count = 0;
    int loop_count = 0;

    for (Loop *L : LI) {
      // Ignore loops we cannot decouple.
      if (!DLP.hasWork(L))
        continue;
      for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
           BI != BE; ++BI, ++bb_count) {
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
          int col = Loc->getColumn();

          lines.push_back(line);
          cols.push_back(col);
          BBs.push_back(bb_count);
          Loops.push_back(loop_count);

          branchLines.push_back(dyn_cast<BranchInst>(&Inst) ? true : false);
          workLines.push_back(DLP.isWork(Inst, L) ? true : false);
          iterLines.push_back(DLP.isIter(Inst, L) ? true : false);
        }
      }
      ++loop_count;
    }

    if (File != nullptr) {
      int prevLoop = -1;
      raw_os_ostream roos(cout);
      for (int i = 0; i < lines.size(); ++i) {
        int line = lines[i];
        if (line == -1)
          continue; // Already traversed.

        if (Loops[i] != prevLoop) {
          prevLoop = Loops[i];
          roos << "Loop " << Loops[i] << "\n";
        }
        roos << File->getFilename() << ":" << line;
        int prevBB = -1;
        for (int j = 0; j < lines.size(); ++j) {
          if (lines[j] != line)
            continue;

          // Destructive and messy (and slow).
          lines[j] = -1;
          if (BBs[j] != prevBB) {
            prevBB = BBs[j];
            roos << " ; BB" << BBs[j] << ":";
          }
          roos << " " << cols[j] << ":";
          if (branchLines[j])
            roos << "b-";
          if (workLines[j])
            roos << "w";
          if (iterLines[j])
            roos << "i";
        }
        roos << '\n';
      }
    } else {
      llvm::errs() << "ERROR: No debugging information found!\n";
    }

    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<DecoupleLoopsPass>();
  }
};

void printLocInfo(DIFile *File, const set<unsigned> &lines, raw_os_ostream &roos) {
  if (File != nullptr) {
    roos << File->getDirectory() << '/' << File->getFilename();
    for (unsigned line : lines) {
      roos << ' ' << line;
    }
    roos << '\n';
  }
}

char DecoupleLoopsPrinter::ID = 0;
RegisterPass<DecoupleLoopsPrinter> DecoupleLoopsPrinterRegister(
    "print-decouple-loops", "Print the result of the -decouple-loops pass");
}
