#include "effects/volume_effect.h"

void VolumeEffect::ApplyEffect(int16_t* buffer, int length)
{
    for (int i = 0; i < length; ++i)
        buffer[i] /= 2;
}

std::string VolumeEffect::GetName() const
{
    return "Quieten";
}

std::vector<std::string> VolumeEffect::GetPropertyNames() const
{
    return { "Volume" };
}
