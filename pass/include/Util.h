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
}

template<typename T>
void debug(string label, T value) {
  cout << "[DSWP-DEBUG] " << label << ": " << value << '\n';
}

template<>
void debug<Instruction *>(string label, Instruction *inst) {
  cout << "[DSWP-DEBUG] " << label << ": ";
  raw_os_ostream out(cout);
  inst->print(out);
  cout << '\n';
}

template<>
void debug<BasicBlock *>(string label, BasicBlock *BB) {
  cout << "[DSWP-DEBUG] " << label << "\n";
  raw_os_ostream out(cout);
  BB->print(out);
  cout << '\n';
}
