#pragma once
#include "effects/audio_effect.h"
#include <complex>
#include <array>

class EqualiserEffect : public AudioEffect
{
public:
    EqualiserEffect();
    void ApplyEffect(Packet& packet) override;
    std::string GetName() const override;

private:
    void ModifySamples(std::vector<float>& samples, const float frequency) const;

    void ModifyFrequencies(
        std::vector<std::complex<float>>& fft_output,
        const int frequency
    ) const;

    void ModifyMagnitude(
        std::complex<float>& number,
        const float multiplier
    ) const;
};
