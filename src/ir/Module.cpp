#include "Module.h"

using namespace cs241c;

Module::Module(const std::string &ModuleName) : Name(ModuleName) {}

const std::string Module::getIdentifier() const { return Name; }