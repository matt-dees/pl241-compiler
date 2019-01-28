#include "Filesystem.h"
#include <stdexcept>

#if WIN32

#include <Windows.h>
#include <string>

bool cs241c::fileExists(std::string_view File) {
  return GetFileAttributes(File.data()) != INVALID_FILE_ATTRIBUTES;
}

std::vector<std::string> cs241c::listFiles(std::string_view Directory) {
  std::vector<std::string> Entries;

  HANDLE Dir;
  WIN32_FIND_DATA Buf;

  if ((Dir = FindFirstFile((std::string(Directory) + "/*").c_str(), &Buf)) ==
      INVALID_HANDLE_VALUE) {
    throw std::runtime_error(std::string("Directory ") + Directory.data() +
                             " does not exist.");
  }

  do {
    bool IsDirectory = (Buf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    if (Buf.cFileName[0] == '.' || IsDirectory)
      continue;

    Entries.push_back(Buf.cFileName);
  } while (FindNextFile(Dir, &Buf));

  FindClose(Dir);

  return Entries;
}

void cs241c::removeFile(std::string_view File) { DeleteFile(File.data()); }

#else

#include <dirent.h>
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

#endif
