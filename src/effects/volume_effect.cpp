#include "effects/volume_effect.h"

VolumeEffect::VolumeEffect()
{
    properties["volume"] = new FloatingPointProperty(0.5f, 0.01f, 1.0f);
}

void VolumeEffect::ApplyEffect(Packet& packet)
{
    for (float& sample : packet.current_samples)
        sample *= GetProperty<float>("volume");
}

std::string VolumeEffect::GetName() const
{
    return "Volume";
}
