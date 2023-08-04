#pragma once
#include "effects/audio_effect.h"
#include <random>

class NoiseEffect : public AudioEffect
{
public:
    void ApplyEffect(Packet& packet) override;
    std::string GetName() const override;
    std::vector<std::string> GetPropertyNames() const override;

private:
    std::default_random_engine random_generator;
    std::uniform_real_distribution<float> random_distribution { -1.0f, 1.0f };
};
