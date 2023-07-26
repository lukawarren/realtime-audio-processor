#include "effects/volume_effect.h"

void VolumeEffect::ApplyEffect(int16_t* buffer, int length) const
{
    for (int i = 0; i < length; ++i)
        buffer[i] = int16_t((float)buffer[i] * 0.1f);
}

std::string VolumeEffect::GetName() const
{
    return "Quieten";
}

std::string VolumeEffect::GetDescription() const
{
    return "Reduces the audio's volume";
}
