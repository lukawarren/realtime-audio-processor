#include "effects/pitch_effect.h"
#include "effects/fft.h"

PitchEffect::PitchEffect()
{
    properties["pitch"] = Property(1.5f, 0.0f, 5.0f);
}

void PitchEffect::ModifyFrequencies(
    std::vector<std::complex<float>>& fft_output,
    const int frequency
) const
{
    const float pitch = GetProperty("pitch");
    auto new_fft_output = fft_output;

    for (size_t i = 0; i < fft_output.size(); ++i)
    {
        // Shift the frequency by the pitch amount
        const size_t new_bin_index = size_t((float)i / pitch);

        // Confine to within bounds
        if (new_bin_index >= 0 && new_bin_index < fft_output.size())
            new_fft_output[i] = fft_output[new_bin_index];
        else
            new_fft_output[i] = std::complex<float>(0.0f, 0.0f);
    }

    fft_output = new_fft_output;
}

std::string PitchEffect::GetName() const
{
    return "Pitch";
}
