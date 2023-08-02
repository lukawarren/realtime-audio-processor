#include "effects/volume_effect.h"

void VolumeEffect::ApplyEffect(
    std::vector<float>& previous_samples,
    std::vector<float>& current_samples,
    std::vector<float>& next_samples,
    const int frequency
)
{
    for (float& sample : current_samples)
        sample /= 2.0f;
}

std::string VolumeEffect::GetName() const
{
    return "Quieten";
}

std::vector<std::string> VolumeEffect::GetPropertyNames() const
{
    return { "Volume" };
}
