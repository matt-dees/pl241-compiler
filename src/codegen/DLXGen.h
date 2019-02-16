#ifndef CS241C_CODEGEN_DLXGEN_H
#define CS241C_CODEGEN_DLXGEN_H

#include <cstdint>
#include <vector>

namespace cs241c {
class Module;

std::vector<uint8_t> genDlx(Module &);
} // namespace cs241c

#endif
