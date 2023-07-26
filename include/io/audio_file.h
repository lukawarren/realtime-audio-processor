#pragma once
#include <SDL2/SDL.h>
#include <string>

class AudioFile
{
public:
    AudioFile(const std::string& filename);
    AudioFile(const AudioFile&) = delete;
    ~AudioFile();

    int GetFrequency() const;
    int GetChannels() const;

    // Raw data
    uint8_t* GetData() const;
    uint32_t GetLength() const;

    // Sound samples (signed 16-bit)
    uint16_t* GetSamples() const;
    uint32_t GetSampleCount() const;
    uint16_t MaxSampleValue() const;

private:
    SDL_AudioSpec properties;
    uint32_t length;
    uint8_t* buffer;
};
