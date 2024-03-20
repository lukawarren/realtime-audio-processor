#include "io/audio_file.h"
#include <iostream>

int main()
{
    const auto test_file = [](const std::string& path)
    {
        AudioFile audio_file(path);
        std::cout << "File: " << path << std::endl;
        std::cout << "Frequency: " << audio_file.GetFrequency() << " Hz" << std::endl;
        std::cout << "Channels: " << audio_file.GetChannels() << std::endl;
        std::cout << std::endl;
    };

    test_file("unit-test-audio-1.wav");
    test_file("unit-test-audio-2.wav");
    test_file("unit-test-audio-3.wav");
    return 0;
}