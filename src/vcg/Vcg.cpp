#include "Vcg.h"
#include "Filesystem.h"
#include <stdexcept>

using namespace cs241c;
using namespace std;

void Vcg::writeToFile(const string &OutFilePath) {
  if (fileExists(OutFilePath)) {
    throw runtime_error("VcgGen Error: File already exists: " + OutFilePath);
  }

  std::ofstream VcgFileStream;
  VcgFileStream.open(OutFilePath);
  VcgFileStream << "graph: {";
  writeProperties(VcgFileStream);
  writeGraph(VcgFileStream);
  VcgFileStream << "}";
  VcgFileStream.close();
}

void Vcg::writeProperties(ofstream &OutFileStream) {
  OutFileStream << "layoutalgorithm: dfs" << endl;
  OutFileStream << "manhattan_edges: yes" << endl;
}