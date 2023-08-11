#pragma once
#include "effects/audio_effect.h"

class VolumeEffect : public AudioEffect
{
public:
    VolumeEffect();
    void ApplyEffect(Packet& packet) override;
    std::string GetName() const override;
};
