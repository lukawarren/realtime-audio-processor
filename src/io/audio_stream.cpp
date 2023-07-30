#include "io/audio_stream.h"
#include <stdexcept>

constexpr uint16_t buffer_length = 512;

AudioStream::AudioStream(const AudioFile* file, const AtomicLinkedList<AudioEffect>* effects)
{
    // Format of audio data
    SDL_AudioSpec desired_format = {};
    desired_format.freq = file->GetFrequency();
    desired_format.format = AUDIO_S16LSB;
    desired_format.channels = file->GetChannels();
    desired_format.samples = buffer_length;

    /*
        SDL is a C API - it is not written in C++. As such, one cannot simply
        pass a std::function or lambda that captures scope. Instead, the callback
        provided must be able to "decay" to a function pointer, and as such can either be:
        - a static function
        - a lambda that does not capture any variables / state.

        To get around this, the pointer to this class is passed in the "user_data"
        field to a "capture-less" lambda below. This pointer is then used to invoke
        the callback for the corresponding class instance.
    */
    desired_format.userdata = this;
    desired_format.callback = [](void* user_data, uint8_t* buffer, int length)
    {
        AudioStream* class_instance = (AudioStream*)user_data;
        class_instance->OnAudioCallback(buffer, length);
    };

    // Create device - do not allow changes to format
    device = {};
    SDL_AudioSpec actual_format = {};
    device = SDL_OpenAudioDevice(
        NULL,
        0,
        &desired_format,
        &actual_format,
        false
    );

    // Ensure format was as required
    if (device == 0 ||
        actual_format.freq != desired_format.freq ||
        actual_format.format != desired_format.format  ||
        actual_format.channels != desired_format.channels ||
        actual_format.samples != desired_format.samples)
        throw std::runtime_error("Unable to create audio device with desired settings");

    input_buffer = file->GetData();
    input_length = file->GetLength();
    this->effects = effects;
}

void AudioStream::Play()
{
    SDL_PauseAudioDevice(device, false);
    is_playing = true;
}

void AudioStream::Pause()
{
    SDL_PauseAudioDevice(device, true );
    is_playing = false;
}

void AudioStream::OnAudioCallback(uint8_t* buffer, int length)
{
    // If we have no data to provide, return a blank buffer
    if (length == 0 || input_length == 0 ||
        input_buffer == nullptr || input_progress + length > input_length)
    {
        memset(buffer, 0, length);
        input_progress = input_length;
    }
    else
    {
        uint8_t* input = input_buffer + input_progress;
        input_progress += length;

        // Apply effects
        effects->ForEach([&](AudioEffect* effect) {
            effect->ApplyEffect((int16_t*)input, length / sizeof(int16_t));
        });

        // Send to device
        memcpy(buffer, input, length);
    }

    on_progress_changed(GetProgress(), buffer, length);
}

void AudioStream::SetProgressChangedCallback(std::function<void(float, uint8_t*, int)> on_progress_changed)
{
    this->on_progress_changed = on_progress_changed;
}

void AudioStream::SetProgress(const float progress)
{
    // Make sure audio thread isn't currently processing any data!
    SDL_ClearQueuedAudio(device);

    // Change progress but round to nearest multiple of buffer length to avoid artifacts
    this->input_progress = input_length * progress;
    this->input_progress = int(this->input_progress / buffer_length) * buffer_length;
}

float AudioStream::GetProgress() const
{
    return (float)input_progress / (float)input_length;
}

bool AudioStream::IsPlaying() const
{
    return is_playing;
}

AudioStream::~AudioStream()
{
    if (device != 0)
    {
        SDL_ClearQueuedAudio(device);
        SDL_CloseAudioDevice(device);
    }
}