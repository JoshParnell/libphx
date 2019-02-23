#include "Directory.h"
#include "File.h"
#include "PhxMemory.h"
#include "PhxString.h"

#include <stdio.h>

#if WINDOWS
  #include <windows.h>
  #include <direct.h>
  #include "windirent.h"
  #define ChangeDir _chdir
  #define GetCWD _getcwd
  #define RemoveDir _rmdir
#elif POSIX
  #include <dirent.h>
  #include <unistd.h>
  #include <sys/stat.h>
  #define ChangeDir chdir
  #define GetCWD getcwd
  #define RemoveDir rmdir
#endif

struct Directory {
  DIR* handle;
};

Directory* Directory_Open (cstr path) {
  DIR* dir = opendir(path);
  if (!dir)
    return 0;
  Directory* self = MemNew(Directory);
  self->handle = dir;
  return self;
}

void Directory_Close (Directory* self) {
  closedir(self->handle);
  MemFree(self);
}

cstr Directory_GetNext (Directory* self) {
  for (;;) {
    dirent* ent = readdir(self->handle);
    if (!ent)
      return 0;
    if (StrEqual(ent->d_name, ".") || StrEqual(ent->d_name, ".."))
      continue;
    return ent->d_name;
  }
}

bool Directory_Change (cstr cwd) {
  return ChangeDir(cwd) == 0;
}

bool Directory_Create (cstr path) {
#if WINDOWS
  _mkdir(path);
#elif POSIX
  mkdir(path, 0775);
#endif
  return File_IsDir(path);
}

cstr Directory_GetCurrent () {
  static char buffer[FILENAME_MAX];
  if (GetCWD(buffer, sizeof(buffer)) != 0)
    return 0;
  buffer[sizeof(buffer) - 1] = 0;
  return buffer;
}

bool Directory_Remove (cstr path) {
  return RemoveDir(path) == 0;
}
