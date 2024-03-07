#pragma once
#include "effects/fourier_effect.h"

class EqualiserEffect : public FourierEffect
{
public:
    EqualiserEffect();
    std::string GetName() const override;

private:
    void ModifyFrequencies(
        std::vector<std::complex<float>>& fft_output,
        const int frequency
    ) const override;

    void ModifyMagnitude(
        std::complex<float>& number,
        const float multiplier
    ) const;
};
