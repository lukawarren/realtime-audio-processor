#include "effects/noise_effect.h"

NoiseEffect::NoiseEffect()
{
    properties["intensity"] = Property(1.0f, 0.0f, 10.0f);
}

void NoiseEffect::ApplyEffect(Packet& packet)
{
    const float intensity = properties["intensity"].value / 1000.0f;

    for (float& sample : packet.current_samples)
        sample += random_distribution(random_generator) * intensity;
}

std::string NoiseEffect::GetName() const
{
    return "White noise";
}
