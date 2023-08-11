#include "effects/echo_effect.h"

EchoEffect::EchoEffect()
{
    properties["fall-off"] = Property(1.0f, 1.0f, 3.0f);
    properties["delay samples"] = Property(1024 * 20, 1024 * 5, 1024 * 100);
}

void EchoEffect::ApplyEffect(Packet& packet)
{
    // Re-size buffer if value changed
    if (delay_buffer.size() != properties["delay samples"].value)
        delay_buffer.resize(properties["delay samples"].value, 0.0f);

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
        const float fall_off = properties["fall-off"].value;

        // Add increasingly "old" echoes with diminishing volume
        float amplitude = samples[i];
        amplitude += delay_buffer[i + gap * 3] * 0.5f / fall_off;
        amplitude += delay_buffer[i + gap * 2] * 0.25f / fall_off;
        amplitude += delay_buffer[i + gap * 1] * 0.125f / fall_off;
        amplitude += delay_buffer[i + gap * 0] * 0.0625f / fall_off;

        // Prevent clipping
        amplitude /= 2.0f;
        samples[i] = amplitude;
    }
}

std::string EchoEffect::GetName() const
{
    return "Echo";
}
