#pragma once
#include "effects/audio_effect.h"

class VolumeEffect : public AudioEffect
{
public:
    void ApplyEffect(
        std::vector<float>& previous_samples,
        std::vector<float>& current_samples,
        std::vector<float>& next_samples,
        const int frequency
    ) override;
    std::string GetName() const override;
    std::vector<std::string> GetPropertyNames() const override;
};
