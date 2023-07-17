#pragma once
#include <SDL2/SDL.h>
#include <functional>

class AudioStream
{
public:
    AudioStream();
    ~AudioStream();

    void Play();
    void Pause();
    void OnAudioCallback(uint8_t* buffer, int length);

    void SetInputData(uint8_t* buffer, uint32_t length);
    void SetProgressChangedCallback(std::function<void(float)> on_progress_changed);

    float GetProgress() const;

private:
    SDL_AudioDeviceID device;
    SDL_AudioSpec properties;
    uint8_t* input_buffer = nullptr;
    uint32_t input_length;
    uint32_t input_progress = 0;

    // User callback
    std::function<void(float)> on_progress_changed;
};