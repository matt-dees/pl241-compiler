#include "Filesystem.h"
#include <stdexcept>

#if WIN32

#include <Windows.h>
#include <string>

using namespace std;

bool cs241c::fileExists(string_view File) { return GetFileAttributes(File.data()) != INVALID_FILE_ATTRIBUTES; }

vector<string> cs241c::listFiles(string_view Directory) {
  vector<string> Entries;

  HANDLE Dir;
  WIN32_FIND_DATA Buf;

  if ((Dir = FindFirstFile((string(Directory) + "/*").c_str(), &Buf)) == INVALID_HANDLE_VALUE) {
    throw runtime_error(string("Directory ") + Directory.data() + " does not exist.");
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

void cs241c::removeFile(string_view File) { DeleteFile(File.data()); }

#else

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

bool cs241c::fileExists(string_view File) {
  struct stat Buf;
  int Result = stat(File.data(), &Buf);
  return Result == 0;
}

vector<string> cs241c::listFiles(string_view Directory) {
  DIR *Dir = opendir(Directory.data());
  if (!Dir) {
    throw runtime_error(string("Directory ") + Directory.data() + " does not exist.");
  }

  vector<string> Entries;
  struct dirent *Entry;
  while ((Entry = readdir(Dir))) {
    Entries.push_back(Entry->d_name);
  }
  closedir(Dir);
  return Entries;
}

void cs241c::removeFile(string_view File) { unlink(File.data()); }

#endif
