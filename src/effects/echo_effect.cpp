#include "effects/echo_effect.h"

EchoEffect::EchoEffect()
{
    delay_buffer.resize(1024 * 20, 0.0f);
}

void EchoEffect::ApplyEffect(Packet& packet)
{
    std::vector<float>& samples = packet.current_samples;
    const size_t length = samples.size();

    // Shift down current data
    for (size_t i = 0; i < delay_buffer.size() - length; ++i)
        delay_buffer[i] = delay_buffer[i + length];

    // Add current buffer
    for (size_t i = 0; i < length; ++i)
        delay_buffer[delay_buffer.size() - 1 - length + i] = samples[i];

    for (size_t i = 0; i < length; ++i)
    {
        const int gap = 128;

        // Add increasingly "old" echoes with diminishing volume
        float amplitude = samples[i];
        amplitude += delay_buffer[i + gap * 3] * 0.5f;
        amplitude += delay_buffer[i + gap * 2] * 0.25f;
        amplitude += delay_buffer[i + gap * 1] * 0.125f;
        amplitude += delay_buffer[i + gap * 0] * 0.0625f;

        samples[i] = amplitude;
    }
}

std::string EchoEffect::GetName() const
{
    return "Echo";
}
