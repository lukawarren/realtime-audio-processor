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
    const std::vector<std::string>& Items() const;
    void Shuffle();

private:
    std::vector<std::string> filenames;
};