#include "Filesystem.h"
#include <dirent.h>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

bool cs241c::fileExists(std::string_view File) {
  struct stat Buf;
  int Result = stat(File.data(), &Buf);
  return Result == 0;
}

std::vector<std::string> cs241c::listFiles(std::string_view Directory) {
  DIR *Dir = opendir(Directory.data());
  if (!Dir) {
    throw std::runtime_error(std::string("Directory ") + Directory.data() +
                             " does not exist.");
  }

  std::vector<std::string> Entries;
  struct dirent *Entry;
  while ((Entry = readdir(Dir))) {
    Entries.push_back(Entry->d_name);
  }
  closedir(Dir);
  return Entries;
}

void cs241c::removeFile(std::string_view File) { unlink(File.data()); }
