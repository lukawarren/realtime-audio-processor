#pragma once
#include "effects/audio_effect.h"

class VolumeEffect : public AudioEffect
{
public:
    void ApplyEffect(int16_t* buffer, int length) const override;
    std::vector<std::string> GetPropertyNames() const override;
};
