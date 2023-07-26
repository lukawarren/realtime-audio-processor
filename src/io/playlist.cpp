#include "io/playlist.h"
#include <fstream>
#include <istream>
#include <stdexcept>

Playlist::Playlist(const std::vector<std::string>& filenames) :
    filenames(filenames)
{
    // TODO: sort
}

void Playlist::SaveToFile(const std::string& path) const
{
    std::ofstream output_file(path);
    if (!output_file.is_open())
        throw std::runtime_error("unable to open file for writing");

    for (const auto& file : filenames)
        output_file << file << "\n";

    output_file.close();
}

Playlist Playlist::FromFile(const std::string& path)
{
    // Read file line by line
    std::string line;
    std::ifstream input_file(path);
    std::vector<std::string> lines;
    while (std::getline(input_file, line))
        lines.emplace_back(line);

    return Playlist(lines);
}

const std::vector<std::string>& Playlist::Items() const
{
    return filenames;
}
