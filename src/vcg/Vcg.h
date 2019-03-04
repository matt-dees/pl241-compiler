#ifndef CS241C_VCG_H
#define CS241C_VCG_H

#include "FunctionAnalyzer.h"
#include "Module.h"
#include "RegisterAllocator.h"
#include <fstream>
#include <iostream>
#include <string>

namespace cs241c {
class VcgWriter {
public:
  void write(Module &M, FunctionAnalyzer &FA, const std::string &OutFilePath);
  void write(AnnotatedIG &AIG, const std::string &OutFilePath);
};
} // namespace cs241c

#endif
