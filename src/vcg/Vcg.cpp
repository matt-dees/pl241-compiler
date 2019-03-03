#include "Vcg.h"
#include "Filesystem.h"
#include <stdexcept>

using namespace cs241c;
using namespace std;

void Vcg::writeToFile(const string &OutFilePath) {
  if (fileExists(OutFilePath)) {
    throw runtime_error("VcgGen Error: File already exists: " + OutFilePath);
  }

  ofstream VcgFileStream;
  VcgFileStream.open(OutFilePath);
  VcgFileStream << "graph: {";
  writeGraph(VcgFileStream);
  VcgFileStream << "}";
  VcgFileStream.close();
}
