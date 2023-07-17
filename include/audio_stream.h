#pragma once
#include <SDL2/SDL.h>

class AudioStream
{
public:
    AudioStream();
    ~AudioStream();

    void Play();
    void Pause();
    void OnAudioCallback(uint8_t* buffer, int length);

private:
    SDL_AudioDeviceID device;
};