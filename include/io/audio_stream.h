#pragma once
#include <SDL2/SDL.h>
#include <functional>
#include "io/audio_file.h"
#include "data/atomic_linked_list.h"
#include "effects/audio_effect.h"

class AudioStream
{
public:
    AudioStream(const AudioFile* file, const AtomicLinkedList<AudioEffect>* effects);
    AudioStream(const AudioStream&) = delete;
    ~AudioStream();

    void Play();
    void Pause();

    void SetProgressChangedCallback(std::function<void(float, uint8_t*, int)> on_progress_changed);
    void SetProgress(const float progress);
    float GetProgress() const;
    bool IsPlaying() const;

private:
    SDL_AudioDeviceID device = 0;
    SDL_AudioSpec properties;
    void OnAudioCallback(uint8_t* buffer, int length);

    // Playback data
    uint8_t* input_buffer;
    uint32_t input_length;
    uint32_t input_progress = 0;
    bool is_playing = false;

    // User effects
    const AtomicLinkedList<AudioEffect>* effects;

    // User callback
    std::function<void(float, uint8_t*, int)> on_progress_changed =
        [](float _, uint8_t* __, int ___){};
};