#include "Module.h"

using namespace cs241c;

Module::Module(const std::string &ModuleName,
               std::vector<std::unique_ptr<Function>> Functions)
    : Functions(std::move(Functions)), Name(ModuleName) {}

const std::string Module::getIdentifier() const { return Name; }