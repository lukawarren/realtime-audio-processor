#pragma once
#include <string>
#include <vector>

struct File
{
    std::string name;
    size_t size;
};

struct Directory
{
    std::string path;
    std::vector<File> files = {};
};
