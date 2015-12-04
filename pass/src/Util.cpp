#include "Util.h"

#include <iostream>
using std::cout;
using std::endl;
#include <fstream>

#include <vector>
using std::vector;

#include <string>
using std::string;
using std::getline;

#include <sstream>
using std::stringstream;

#include <ostream>
using std::ostream;

#include "llvm/IR/Instructions.h"
using llvm::BranchInst;

#include "llvm/Support/Casting.h"
using llvm::isa;

struct LineLoc {
  const string filename;
  const int line;

  LineLoc(const string filename, const int line)
      : filename(filename), line(line) {}

  LineLoc(const DILocation &DIL)
      : filename(DIL.getDirectory().str() + '/' + DIL.getFilename().str()),
        line(DIL.getLine()) {}

  bool operator<(const LineLoc &other) const { return line < other.line; }

  friend ostream &operator<<(ostream &out, const LineLoc &loc) {
    return out << loc.filename << ':' << loc.line;
  }
};

string getBBSourceCode(const BasicBlock &BB) {
  raw_os_ostream roos(cout);

  vector<LineLoc> locs;
  int lastLine = 0;
  for (auto it = BB.begin(); it != BB.end(); ++it) {
    const Instruction &I = *it;
    if (DILocation *Loc = I.getDebugLoc()) {
      auto jt = it;
      if (BB.end() == ++jt && BB.begin() != it && isa<BranchInst>(I))
        continue;
      if (lastLine < Loc->getLine()) {
        locs.push_back(LineLoc(*Loc));
        lastLine = Loc->getLine();
      }
    }
  }

  if (locs.size() == 0) {
    return "";
  }

  std::ifstream ifs(locs[0].filename, std::ifstream::in);

  stringstream ss;
  int lineNo = 0;
  string line;
  auto it = locs.begin();
  while (it != locs.end()) {
    getline(ifs, line);
    lineNo++;
    if (it->line == lineNo) {
      ++it;
      ss << lineNo << ": " << line << "\\l";
    }
  }
  return ss.str();
}

void log_print_impl() {
  cout << endl;
  return;
}
