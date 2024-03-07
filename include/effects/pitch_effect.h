#pragma once
#include "effects/fourier_effect.h"

class PitchEffect : public FourierEffect
{
public:
    PitchEffect();
    std::string GetName() const override;

private:
    void ModifyFrequencies(
        std::vector<std::complex<float>>& fft_output,
        const int frequency
    ) const override;
};
