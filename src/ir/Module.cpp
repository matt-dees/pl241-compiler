#include "Module.h"

using namespace cs241c;

Module::Module(const std::string &ModuleName)
    : Functions(std::vector<Function>({})), Name(ModuleName) {}

const std::string Module::getIdentifier() const { return Name; }