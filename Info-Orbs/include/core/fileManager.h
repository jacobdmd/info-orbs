#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <Arduino.h>
#include <LittleFS.h>
#define FORMAT_LITTLEFS_IF_FAILED true

class FileManager {
  public:
    String readFile(String filename);
    bool writeFile(String filename, String data);
    bool mountFS();
};
#endif