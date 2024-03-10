#include "effects/noise_effect.h"

NoiseEffect::NoiseEffect()
{
    properties["intensity"] = new FloatingPointProperty(1.0f, 0.0f, 10.0f);
}

void NoiseEffect::ApplyEffect(Packet& packet)
{
    const float intensity = GetProperty<float>("intensity") / 100.0f;

    for (float& sample : packet.current_samples)
    {
        // Add noise then avoid clipping (recall that a sound wave played
        // by a speaker cannot range from more than -100% to +100% displacement,
        // and trying to do so would result in a form of audio distortion)
        sample += random_distribution(random_generator) * intensity;
        sample = std::max(sample, -1.0f);
        sample = std::min(sample, 1.0f);
    }
}

std::string NoiseEffect::GetName() const
{
    return "White noise";
}
