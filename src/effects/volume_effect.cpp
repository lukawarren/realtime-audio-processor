#include "effects/volume_effect.h"

void VolumeEffect::ApplyEffect(Packet& packet)
{
    for (float& sample : packet.current_samples)
        sample /= 2.0f;
}

std::string VolumeEffect::GetName() const
{
    return "Quieten";
}
