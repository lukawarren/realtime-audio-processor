#include "effects/noise_effect.h"

NoiseEffect::NoiseEffect()
{
    properties["intensity"] = Property(1.0f, 0.0f, 10.0f);
}

void NoiseEffect::ApplyEffect(Packet& packet)
{
    const float intensity = properties["intensity"].value / 100.0f;

    for (float& sample : packet.current_samples)
    {
        // Add noise then avoid clipping
        sample += random_distribution(random_generator) * intensity;
        sample = std::max(sample, -1.0f);
        sample = std::min(sample, 1.0f);
    }
}

std::string NoiseEffect::GetName() const
{
    return "White noise";
}
