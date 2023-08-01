#include "effects/volume_effect.h"

void VolumeEffect::ApplyEffect(std::vector<float>& samples)
{
    for (float& sample : samples)
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
