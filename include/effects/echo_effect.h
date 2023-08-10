#pragma once
#include "effects/audio_effect.h"
#include <array>

class EchoEffect : public AudioEffect
{
public:
    EchoEffect();

    void ApplyEffect(Packet& packet) override;
    std::string GetName() const override;

private:
    std::vector<float> delay_buffer = {};
};
