// Implemented in Util.cpp.

#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;
#include "llvm/IR/Instruction.h"
using llvm::Instruction;
#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;

#include <string>
using std::string;
#include <iostream>
using std::cout;

void info(string msg) {
  cout << "[DSWP] " << msg << '\n';

  return;
}

template<typename T>
void debug(const string &label, const T value) {
  cout << "[DSWP-DEBUG] " << label << ": " << value << '\n';

  return;
}

template<>
void debug<const Instruction *>(const string &label, const Instruction *inst) {
  cout << "[DSWP-DEBUG] " << label << ": ";
  raw_os_ostream out(cout);
  inst->print(out);
  cout << '\n';

  return;
}

template<>
void debug<const BasicBlock *>(const string &label, const BasicBlock *BB) {
  cout << "[DSWP-DEBUG] " << label << "\n";
  raw_os_ostream out(cout);
  BB->print(out);
  cout << '\n';

  return;
}
