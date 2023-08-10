#pragma once
#include "effects/audio_effect.h"
#include <complex>
#include <array>

class BassEffect : public AudioEffect
{
public:
    BassEffect();
    void ApplyEffect(Packet& packet) override;
    std::string GetName() const override;

private:
    std::array<std::vector<float>, 3> CreateWindows(Packet& packet) const;
    void PerformBassBoost(
        std::vector<std::complex<float>>& fft_output,
        const int frequency
    ) const;
};
