#include "audio_stream.h"
#include <stdexcept>

AudioStream::AudioStream() {}

AudioStream::AudioStream(const int frequency, const SDL_AudioFormat format, int channels)
{
    // Format of audio data
    SDL_AudioSpec desired_format = {};
    desired_format.freq = frequency;
    desired_format.format = AUDIO_S16LSB;
    desired_format.channels = channels;
    desired_format.samples = 4096;

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
}

void AudioStream::Play()
{
    SDL_PauseAudioDevice(device, false);
}

void AudioStream::Pause()
{
    SDL_PauseAudioDevice(device, true );
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
        memcpy(buffer, input_buffer + input_progress, length);
        input_progress += length;
    }

    on_progress_changed(GetProgress());
}

void AudioStream::SetInputData(uint8_t* buffer, uint32_t length)
{
    input_buffer = buffer;
    input_length = length;
    input_progress = 0;
}

void AudioStream::SetProgressChangedCallback(std::function<void(float)> on_progress_changed)
{
    this->on_progress_changed = on_progress_changed;
}

float AudioStream::GetProgress() const
{
    return (float)input_progress / (float)input_length;
}

AudioStream::~AudioStream()
{
    if (device != 0)
        SDL_CloseAudioDevice(device);
}