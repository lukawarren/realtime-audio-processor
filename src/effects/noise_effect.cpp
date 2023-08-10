#include "effects/noise_effect.h"

void NoiseEffect::ApplyEffect(Packet& packet)
{
    const float intensity = 0.01f;

    for (float& sample : packet.current_samples)
        sample += random_distribution(random_generator) * intensity;
}

std::string NoiseEffect::GetName() const
{
    return "White noise";
}
