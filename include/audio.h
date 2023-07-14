#pragma once
#include <string>
#include <vector>
#include <iostream>

struct AudioFile
{
    std::string name;
    size_t size;
};

struct AudioDirectory
{
    std::string path;
    std::vector<AudioFile> files = {};
};
