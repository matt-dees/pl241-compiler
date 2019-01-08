#ifndef CS241C_VCGGEN_H
#define CS241C_VCGGEN_H

#include "Module.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

namespace cs241c {
class VcgGen {
public:
  explicit VcgGen(const Module &InputModule);

  // Generates .vcg file that serves as input to the VCG program.
  // Throws exception if unable to generate file correctly.
  void generate(const std::string &OutFilePath);

private:
  void writeGraph();
  void writeBasicBlock(const BasicBlock &BB);
  void writeEdge();
  void writeProperties();

  std::ofstream VcgFileStream;
  const Module InputModule;
};
} // namespace cs241c
#endif // CS241C_VCGGEN_H
