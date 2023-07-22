#include "audio_file.h"
#include <stdexcept>

AudioFile::AudioFile(const std::string& filename)
{
    SDL_LoadWAV(filename.c_str(), &properties, &buffer, &length);

    if (properties.format != AUDIO_S16LSB)
        throw std::runtime_error("Unsupported audio format");
}

int AudioFile::GetFrequency() const
{
    return properties.freq;
}

int AudioFile::GetChannels() const
{
    return properties.channels;
}

uint8_t* AudioFile::GetData() const
{
    return buffer;
}

uint32_t AudioFile::GetLength() const
{
    return length;
}

AudioFile::~AudioFile()
{
    SDL_FreeWAV(buffer);
}
