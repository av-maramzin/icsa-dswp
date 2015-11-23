// Implemented in Util.cpp.

#ifndef ICSA_DSWP_UTIL_H
#define ICSA_DSWP_UTIL_H


#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;
#include "llvm/IR/Instruction.h"
using llvm::Instruction;
#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;

#include <string>
using std::string;
#include <iostream>
using std::ostream;
using std::cout;
using std::endl;

enum class log_severity : unsigned int {
  INFO,
  DEBUG
};

template<const string &prefix>
class logger {
public:
  template<log_severity severity, typename... Ts>
  void print(Ts... args) {
    switch(severity) {
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

    cout << prefix << ": ";

    print_impl(args...);

    return;
  }

private:
  void print_impl() {
    cout << endl;
    return;
  }

  template<typename T, typename... Ts>
  void print_impl(T v, Ts... args) {
    cout << v;
    print_impl(args...);

    return;
  }

  template<typename... Ts>
  void print_impl(BasicBlock *BB, Ts... args) {
    raw_os_ostream roos(cout);
    BB->print(roos);
    print_impl(args...);

    return;
  }

  template<typename... Ts>
  void print_impl(Instruction *inst, Ts... args) {
    raw_os_ostream roos(cout);
    inst->print(roos);
    print_impl(args...);

    return;
  }
};

extern const string log_prefix{"ICSA-DSWP"};

static logger<log_prefix> dswp_logger;

#define LOG_INFO dswp_logger.print<log_severity::INFO>
#define LOG_DEBUG dswp_logger.print<log_severity::DEBUG>


#endif // ICSA_DSWP_UTIL_H

