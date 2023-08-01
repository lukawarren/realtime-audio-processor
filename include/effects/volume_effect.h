#pragma once
#include "effects/audio_effect.h"

class VolumeEffect : public AudioEffect
{
public:
    void ApplyEffect(std::vector<float>& samples) override;
    std::string GetName() const override;
    std::vector<std::string> GetPropertyNames() const override;
};
