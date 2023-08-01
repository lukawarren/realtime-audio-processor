#include "effects/echo_effect.h"
#include "effects/fft.h"

EchoEffect::EchoEffect()
{
    delay_buffer.resize(1024 * 20, 0.0f);
}

void EchoEffect::ApplyEffect(std::vector<float>& samples)
{
    const size_t length = samples.size();

    // Shift down current data
    for (size_t i = 0; i < delay_buffer.size() - length; ++i)
        delay_buffer[i] = delay_buffer[i + length];

    // Add current buffer
    for (size_t i = 0; i < length; ++i)
        delay_buffer[delay_buffer.size() - 1 - length + i] = samples[i];

    for (size_t i = 0; i < length; ++i)
    {
        /*
            There is a very real chance that after combining multiple "waves",
            the resultant volume will be higher than the 16-bits can represent.
            For some songs, this manifests itself as occasional "crackling". For
            others, the entire song is rendered unintelligible.

            Luckily, if you add 1 + 1/2 + 1/4 + 1/8 + 1/16... (as we're doing),
            you get 2! So if we combine all the "waves" then halve the result,
            the volume should stay roughly equal!
        */

        const int gap = 128;

        float amplitude = samples[i];
        amplitude += delay_buffer[i + gap * 3] * 0.5f;
        amplitude += delay_buffer[i + gap * 2] * 0.25f;
        amplitude += delay_buffer[i + gap * 1] * 0.125f;
        amplitude += delay_buffer[i + gap * 0] * 0.0625f;

        // samples[i] = amplitude / 2.0f;
        samples[i] = amplitude;
    }
}

std::string EchoEffect::GetName() const
{
    return "Echo";
}

std::vector<std::string> EchoEffect::GetPropertyNames() const
{
    return { "Lag", "Gap", "Damper" };
}
