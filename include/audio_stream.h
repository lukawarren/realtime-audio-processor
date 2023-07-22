#pragma once
#include <SDL2/SDL.h>
#include <functional>
#include "audio_file.h"

class AudioStream
{
public:
    AudioStream(const AudioFile* file);
    AudioStream(const AudioStream&) = delete;
    ~AudioStream();

    void Play();
    void Pause();
    void OnAudioCallback(uint8_t* buffer, int length);

    void SetProgressChangedCallback(std::function<void(float)> on_progress_changed);
    void SetProgress(const float progress);
    float GetProgress() const;
    bool IsPlaying() const;

private:
    SDL_AudioDeviceID device = 0;
    SDL_AudioSpec properties;

    // Playback data
    uint8_t* input_buffer;
    uint32_t input_length;
    uint32_t input_progress = 0;
    bool is_playing = false;

    // User callback
    std::function<void(float)> on_progress_changed = [](float _){};
};