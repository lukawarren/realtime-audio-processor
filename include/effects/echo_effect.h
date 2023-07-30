#pragma once
#include "effects/audio_effect.h"
#include <array>

class EchoEffect : public AudioEffect
{
public:
    EchoEffect();

    void ApplyEffect(int16_t* buffer, int length) override;
    std::string GetName() const override;
    std::vector<std::string> GetPropertyNames() const override;

private:
    std::vector<int16_t> delay_buffer = {};
};
