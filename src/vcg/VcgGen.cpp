#include "VcgGen.h"
#include <filesystem>

#include <stdexcept>

using namespace cs241c;
namespace fs = std::filesystem;

VcgGen::VcgGen(const Module &InputModule) : InputModule(InputModule) {}

void VcgGen::generate(const std::string &OutFilePath) {
  if (fs::exists(OutFilePath)) {
    throw std::runtime_error("VcgGen Error: File already exists: " +
                             OutFilePath);
  }

  VcgFileStream.open(OutFilePath);
  writeGraph();
  VcgFileStream.close();
}

void VcgGen::writeGraph() {
  VcgFileStream << "graph : {" << std::endl;
  VcgFileStream << "title : "
                << "\"" << InputModule.getIdentifier() << "\"" << std::endl;

  writeProperties();

  for (auto &Function : InputModule.Functions) {
    const std::string currentFunctionName = Function.Name;
  }
  VcgFileStream << "}";
}
