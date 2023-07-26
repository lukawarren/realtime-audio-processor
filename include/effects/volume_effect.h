#pragma once
#include "effects/audio_effect.h"

class VolumeEffect : public AudioEffect
{
public:
    void ApplyEffect(int16_t* buffer, int length) const override;
    std::string GetName() const override;
    std::string GetDescription() const override;
};
