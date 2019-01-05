#ifndef CS241C_FRONTEND_IRGENCONTEXT_H
#define CS241C_FRONTEND_IRGENCONTEXT_H

#include "Value.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace cs241c {
class IrGenContext {
  std::vector<std::unique_ptr<Value>> Values;
  std::unordered_map<std::string, std::unique_ptr<Value>> LocalVariables;
  std::unordered_map<std::string, std::unique_ptr<Value>> GlobalVariables;

public:
  Value *makeConstant(int Val);

  Value *lookupVariable(const std::string &Ident);
};
} // namespace cs241c

#endif
