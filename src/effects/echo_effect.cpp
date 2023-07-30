#include "effects/echo_effect.h"

EchoEffect::EchoEffect()
{
    delay_buffer.resize(1024*20, 0);
}

void EchoEffect::ApplyEffect(int16_t* buffer, int length)
{
    // Shift down current data
    for (size_t i = 0; i < delay_buffer.size() - length; ++i)
        delay_buffer[i] = delay_buffer[i + length] * 1.0f;

    // Add current buffer
    for (int i = 0; i < length; ++i)
        delay_buffer[delay_buffer.size() - 1 - length + i] = buffer[i];

    for (int i = 0; i < length; ++i)
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

        int32_t amplitude = buffer[i];
        amplitude += delay_buffer[i + gap * 3] * 0.5f;
        amplitude += delay_buffer[i + gap * 2] * 0.25f;
        amplitude += delay_buffer[i + gap * 1] * 0.125f;
        amplitude += delay_buffer[i + gap * 0] * 0.0625f;

        buffer[i] = int16_t(amplitude / 2);
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
