#include "core/fileManager.h"

String FileManager::readFile(String filename)
{
  File file = LittleFS.open(filename);

  if(!file) {
      Serial.println("Unable to open file: " + filename);
      return "";
  }

  String fileText = "";
  
  while(file.available()) {
    fileText = file.readString();
  }

  file.close();
  
  return fileText;
}

bool FileManager::writeFile(String filename, String data)
{
  File file = LittleFS.open(filename, FILE_WRITE);
  if(!file)
  {
    Serial.println("Unable to open file to write: " + filename);
    return false;
  }
  
  if(file.print(data))
  {

  } else {
    Serial.println("Unable to write file: " + filename);
    file.close();
    return false;
  }

  file.close();
  
  return true;
}

bool FileManager::mountFS()
{
  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
  {
    Serial.println("Failed to mount LittleFS.");
    return false;
  } 
  
  Serial.println("LittleFS mounted.");

  return true;
}