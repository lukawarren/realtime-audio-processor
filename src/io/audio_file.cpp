#include "io/audio_file.h"
#include <stdexcept>

AudioFile::AudioFile(const std::string& filename)
{
    if (SDL_LoadWAV(filename.c_str(), &properties, &buffer, &length) == NULL)
        throw std::runtime_error("Could not open audio file " + filename);

    if (properties.format != AUDIO_S16LSB)
        throw std::runtime_error("Unsupported audio format when trying to load " + filename);
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

uint16_t* AudioFile::GetSamples() const
{
    return (uint16_t*)buffer;
}

uint32_t AudioFile::GetSampleCount() const
{
    return length / 2;
}

uint16_t AudioFile::MaxSampleValue() const
{
    // Max value for 16-bit signed integer :)
    return 32768;
}

AudioFile::~AudioFile()
{
    SDL_FreeWAV(buffer);
}
