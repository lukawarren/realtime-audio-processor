#pragma once
#include <string>
#include <vector>

class Playlist
{
public:
    Playlist(const std::vector<std::string>& filenames);
    void SaveToFile(const std::string& path) const;
    static Playlist FromFile(const std::string& path);
    const std::vector<std::string>& Items() const;

private:
    const std::vector<std::string> filenames;
};