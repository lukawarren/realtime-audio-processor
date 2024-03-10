#pragma once
#include <string>
#include <vector>
#include <optional>

class Playlist
{
public:
    Playlist(const std::vector<std::string>& filenames);

    void SaveToFile(const std::string& path) const;
    static std::optional<Playlist> FromFile(const std::string& path);
    std::vector<std::string>& Items();
    void Shuffle();

private:
    std::vector<std::string> filenames;
};