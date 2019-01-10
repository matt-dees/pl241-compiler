#include "Module.h"

using namespace cs241c;

Module::Module(std::string ModuleName,
               std::vector<std::unique_ptr<GlobalVariable>> &&Globals,
               std::vector<std::unique_ptr<Function>> &&Functions)
    : Name(move(ModuleName)), Globals(move(Globals)),
      Functions(std::move(Functions)) {}
std::string Module::getIdentifier() const { return Name; }
