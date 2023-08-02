#pragma once
#include "effects/audio_effect.h"

class VolumeEffect : public AudioEffect
{
public:
    void ApplyEffect(Packet& packet) override;
    std::string GetName() const override;
    std::vector<std::string> GetPropertyNames() const override;
};
