#include "io/playlist.h"
#include "data/merge_sort.h"
#include <fstream>
#include <istream>
#include <stdexcept>

Playlist::Playlist(const std::vector<std::string>& filenames)
{
    // Sort
    this->filenames = MergeSort(filenames);
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

std::optional<Playlist> Playlist::FromFile(const std::string& path)
{
    // Check file actually exists
    std::ifstream input_file(path);
    if (!input_file.good())
        return std::nullopt;

    // Read file line by line
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(input_file, line))
        lines.emplace_back(line);

    // Verify playlist is not empty
    if (lines.size() == 0)
        return std::nullopt;

    // Verify each file actually exists
    for (const auto& filename : lines)
    {
        std::ifstream file(filename);
        if (!file.good())
        {
            // File does not exist or cannot be read - we cannot return a
            // valid playlist!
            return std::nullopt;
        }
    }

    return Playlist(lines);
}

std::vector<std::string>& Playlist::Items()
{
    return filenames;
}

void Playlist::Shuffle()
{
    /*
        Perform a "Fisher–Yates" shuffle
        https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle.

        Use the current time as the seed.
    */

    const std::size_t size = filenames.size();
    srand(time(NULL));

    // Iterate through vector in reverse
    for (std::size_t i = size - 1; i > 0; --i)
    {
        // Randomly pick a random index from 0 to N (inclusive) and swap with i
        const std::size_t index = rand() % (i + 1);
        std::swap(filenames[i], filenames[index]);
    }
}
