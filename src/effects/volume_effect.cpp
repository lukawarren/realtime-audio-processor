#include "effects/volume_effect.h"

void VolumeEffect::ApplyEffect(int16_t* buffer, int length) const
{
    for (int i = 0; i < length; ++i)
        buffer[i] /= 2;
}

std::string VolumeEffect::GetName() const
{
    return "Quieten";
}

std::string VolumeEffect::GetDescription() const
{
    return "Reduces the audio's volume";
}
