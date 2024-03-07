#pragma once
#include "effects/audio_effect.h"
#include <complex>
#include <array>

class FourierEffect : public AudioEffect
{
public:
    FourierEffect() {}
    void ApplyEffect(Packet& packet) override;

protected:
    virtual void ModifyFrequencies(
        std::vector<std::complex<float>>& fft_output,
        const int frequency
    ) const = 0;

private:
    void ModifySamples(std::vector<float>& samples, const float frequency) const;
};
