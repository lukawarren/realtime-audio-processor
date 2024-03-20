#include "io/playlist.h"
#include <iostream>

int main()
{
    Playlist playlist({"a", "b", "c", "d", "e", "f", "g"});

    for (int i = 0; i < 10; ++i)
    {
        playlist.Shuffle();
        for (const auto& item : playlist.Items())
            std::cout << item << ", ";
        std::cout << std::endl;
    }

    return 0;
}