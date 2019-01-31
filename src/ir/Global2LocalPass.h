#ifndef CS241C_IR_GLOBAL2LOCALPASS_H
#define CS241C_IR_GLOBAL2LOCALPASS_H

#include <unordered_map>

namespace cs241c {
class BasicBlock;
class Function;
class GlobalVariable;
class Module;

class Global2LocalPass {
  std::unordered_multimap<Function *, GlobalVariable *> LoadGlobals;
  std::unordered_multimap<Function *, GlobalVariable *> StoreGlobals;

public:
  void run(Module *);
};
} // namespace cs241c

#endif
