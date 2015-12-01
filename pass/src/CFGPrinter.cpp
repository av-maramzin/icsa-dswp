#include <string>
using std::string;
#include <system_error>
using std::error_code;

#include "llvm/IR/Module.h"
using llvm::Module;
#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassRegistry.h"
using llvm::RegisterPass;
#include "llvm/Support/raw_ostream.h"
using llvm::errs;
using llvm::raw_fd_ostream;
using llvm::raw_ostream;
#include "llvm/Support/FileSystem.h"
using llvm::sys::fs::F_Text;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;
#include "llvm/Support/GraphWriter.h"
using llvm::WriteGraph;

#include "FuncGraphTraits.h"

namespace icsa {

// A custom printer that prints source code rather than LLVM code.
struct CFGPrinter : public FunctionPass {
  static char ID;
  CFGPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    string Filename = ("cfg." + F.getName() + ".dot").str();
    errs() << "Writing '" << Filename << "'...";

    error_code EC;
    raw_fd_ostream File(Filename, EC, F_Text);

    if (!EC) {
      const PrintFunction PF{F};
      WriteGraph(File, (const PrintFunction *)&PF);
    } else {
      errs() << "  error opening file for writing!";
    }
    errs() << "\n";

    return false;
  }

  void print(raw_ostream &OS, const Module * = nullptr) const override {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<ControlDependenceGraph>();
  }
};

char CFGPrinter::ID = 0;
RegisterPass<CFGPrinter>
    CFGPrinterRegister("icsa-dot-cfg", "Print CDG of function to 'dot' file");

}
