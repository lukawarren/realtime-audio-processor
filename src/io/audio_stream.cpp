#include "io/audio_stream.h"
#include <stdexcept>

constexpr uint16_t buffer_length = 1024;

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
    max_sample_value = file->MaxSampleValue();
    input_frequency = file->GetFrequency();
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
    // Grab pointers to previous, current and next buffers, taking into account
    // the current playback speed!
    int realtime_length = length * input_speed;
    uint8_t* previous_input = input_buffer + input_progress - realtime_length;
    uint8_t* current_input = input_buffer + input_progress;
    uint8_t* next_input = input_buffer + input_progress + realtime_length;
    input_progress += realtime_length;

    // If we have no data to provide, return a blank buffer
    if (!BufferIsInRange(current_input, length))
    {
        memset(buffer, 0, length);
        input_progress = input_length;
    }
    else
    {
        bool previous_is_valid = BufferIsInRange(previous_input, realtime_length);
        bool next_is_valid = BufferIsInRange(next_input, realtime_length);

        // Convert to floats for easier processing
        std::vector<float> empty = {};
        std::vector<float> previous = previous_is_valid ? ConvertBufferToFloats(previous_input, realtime_length) : empty;
        std::vector<float> current = ConvertBufferToFloats(current_input, realtime_length);
        std::vector<float> next = next_is_valid ? ConvertBufferToFloats(next_input, realtime_length) : empty;

        // "Squash" wave from 2x speed, 3x speed, etc. into 1x speed
        previous = ResampleBuffer(previous);
        current = ResampleBuffer(current);
        next = ResampleBuffer(next);

        // Apply effects
        AudioEffect::Packet packet = {
            .previous_samples = previous,
            .current_samples = current,
            .next_samples = next,
            .frequency = input_frequency
        };
        effects->ForEach([&](AudioEffect* effect) {
            effect->ApplyEffect(packet);
        });

        // Copy back to buffer
        for (int i = 0; i < length / 2; ++i)
            ((uint16_t*)buffer)[i] = current[i] * max_sample_value;
    }

    on_progress_changed(GetProgress(), buffer, length);
}

bool AudioStream::BufferIsInRange(uint8_t* buffer, int length)
{
    return !(
        length == 0 ||
        input_length == 0 ||
        input_buffer == nullptr ||
        buffer < input_buffer ||
        buffer + length > input_buffer + input_length
    );
}

std::vector<float> AudioStream::ConvertBufferToFloats(uint8_t* buffer, int length)
{
    std::vector<float> audio;
    audio.reserve(length / 2);
    for (int i = 0; i < length / 2; ++i)
        audio.emplace_back(
            float(*((int16_t*)buffer + i)) / (float)max_sample_value
        );
    return audio;
}

std::vector<float> AudioStream::ResampleBuffer(std::vector<float>& buffer)
{
    // Allocate output buffer
    std::vector<float> output;
    size_t targetSize = (size_t)std::round(buffer.size() / input_speed);
    output.reserve(targetSize);

    // Copy input buffer to output in "input_speed"-sized steps
    float currentIndex = 0.0f;
    while (currentIndex < buffer.size())
    {
        output.push_back(buffer[std::round(currentIndex)]);
        currentIndex += input_speed;
    }

    return output;
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

void AudioStream::SetSpeed(const float speed)
{
    input_speed = speed;
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