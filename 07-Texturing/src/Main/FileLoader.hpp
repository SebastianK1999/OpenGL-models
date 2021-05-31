#ifndef FILE_LOADER_INCLUDED
#define FILE_LOADER_INCLUDED

#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>

class FileLoader{
private:
    std::vector<std::vector<std::string>> files;
    std::vector<std::vector<FILE *>> filesOpened;
public:
    FileLoader();
    ~FileLoader();
    std::ifstream getFile(int, int);
    FILE * getFilePointer(int, int);
    void getHeight(int16_t& n, size_t verDeg, size_t horDeg, size_t verI, size_t horI);

};

#endif