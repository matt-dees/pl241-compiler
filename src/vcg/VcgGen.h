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
  explicit VcgGen(std::unique_ptr<Module> InputModule);

  // Generates .vcg file that serves as input to the VCG program.
  // Throws exception if unable to generate file correctly.
  void generate(const std::string &OutFilePath);

private:
  void writeGraph();
  void writeBasicBlock(BasicBlock *BB, const std::string &Title);
  void writeEdge(BasicBlock *Source, BasicBlock *Destination);
  void writeProperties();
  void writeFunction(Function *F);

  std::string indent();
  std::ofstream VcgFileStream;
  std::unique_ptr<Module> InputModule;
};
} // namespace cs241c
#endif // CS241C_VCGGEN_H
