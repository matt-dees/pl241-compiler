#ifndef CS241C_UTIL_FILESYSTEM_H
#define CS241C_UTIL_FILESYSTEM_H

#include <string>
#include <string_view>
#include <vector>

namespace cs241c {
bool fileExists(std::string_view File);
std::vector<std::string> listFiles(std::string_view Directory);
void removeFile(std::string_view File);
} // namespace cs241c

#endif
