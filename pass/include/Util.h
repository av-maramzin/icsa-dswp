// Implemented in Util.cpp.

#ifndef ICSA_DSWP_UTIL_H
#define ICSA_DSWP_UTIL_H

#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;
#include "llvm/IR/Instruction.h"
using llvm::Instruction;
#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;
#include "llvm/IR/DebugInfoMetadata.h"
using llvm::DILocation;
#include "llvm/IR/DebugLoc.h"
using llvm::DebugLoc;

#include <utility>
using std::forward;
#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::endl;

string getBBSourceCode(const BasicBlock &BB);

enum class log_severity : unsigned int { INFO, DEBUG };

// Implemented in Util.cpp.
void log_print_impl();

template <typename... Ts>
void log_print_impl(const BasicBlock *BB, Ts... args) {
  raw_os_ostream roos(cout);
  BB->print(roos);
  log_print_impl(args...);

  return;
}

template <typename... Ts> void log_print_impl(BasicBlock *BB, Ts... args) {
  log_print_impl(static_cast<const BasicBlock *>(BB), forward<Ts>(args)...);

  return;
}

template <typename... Ts>
void log_print_impl(const Instruction *inst, Ts... args) {
  raw_os_ostream roos(cout);
  inst->print(roos);
  log_print_impl(args...);

  return;
}

template <typename... Ts> void log_print_impl(Instruction *inst, Ts... args) {
  log_print_impl(static_cast<const Instruction *>(inst), forward<Ts>(args)...);

  return;
}

// general case

template <typename T, typename... Ts> void log_print_impl(T v, Ts... args) {
  cout << v;
  log_print_impl(args...);

  return;
}

//

template <log_severity severity, typename... Ts> void log_print(Ts &&... args) {
  switch (severity) {
  case (log_severity::INFO):
    cout << "[INFO] ";
    break;
  case (log_severity::DEBUG):
    cout << "[DEBUG] ";
    break;
  default:
    cout << "[UNKNOWN] ";
    break;
  }

  cout << "ICSA-DSWP: ";

  log_print_impl(forward<Ts>(args)...);

  return;
}

#define LOG_INFO log_print<log_severity::INFO>
#define LOG_DEBUG log_print<log_severity::DEBUG>

#endif // ICSA_DSWP_UTIL_H
