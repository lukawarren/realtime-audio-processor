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
    std::array<std::vector<float>, 3> CreateWindows(Packet& packet) const;

    void ModifyWindow(std::vector<float>& window, const float frequency) const;

    void ModifyFrequencies(
        std::vector<std::complex<float>>& fft_output,
        const int frequency
    ) const;

    void ModifyMagnitude(
        std::complex<float>& number,
        const float multiplier
    ) const;
};
