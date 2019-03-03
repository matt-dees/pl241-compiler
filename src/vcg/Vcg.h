#ifndef CS241C_VCG_VCGWRITABLE_H
#define CS241C_VCG_VCGWRITABLE_H

#include <fstream>
#include <iostream>
#include <string>

namespace cs241c {
class Vcg {
protected:
  virtual void writeGraph(std::ofstream &OutFileStream) = 0;

public:
  virtual ~Vcg() = default;
  void writeToFile(const std::string &OutFilePath);
};
} // namespace cs241c

#endif
