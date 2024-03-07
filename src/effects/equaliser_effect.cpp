#include "effects/equaliser_effect.h"
#include "effects/fft.h"

EqualiserEffect::EqualiserEffect()
{
    properties["lower frequency"] = Property(20.0f, 0.0f, 20000.0f);
    properties["upper frequency"] = Property(2000.0f, 0.0f, 20000.0f);
    properties["multiplier"] = Property(0.0f, 0.0f, 2.0f);
}

void EqualiserEffect::ModifyFrequencies(
    std::vector<std::complex<float>>& fft_output,
    const int frequency
) const
{
    // Properties
    const float lower_frequency = GetProperty("lower frequency");
    const float upper_frequency = GetProperty("upper frequency");
    const float multiplier = GetProperty("multiplier");

    // Work out "bins"
    const float frequency_resolution = (float)frequency / (float)fft_output.size();
    int lower_bin = lower_frequency / frequency_resolution;
    int upper_bin = upper_frequency / frequency_resolution;

    /*
        Because of the finite bin size, the lower bin may not be 0, even if the
        frequency is. Likewise, the upper bin may not be the maximum it can be
        even if the upper frequency is 20,000 Hz! This is sometimes called
        "spectral leakage" and causes quite a bit of noise. Adding a bit of
        leeway helps fix this for when the lower frequency is 0 Hz, for example.
    */
   lower_bin = std::max(lower_bin - 1, 0);
   upper_bin = std::min(upper_bin + 1, (int)fft_output.size() - 1);

    for (int i = lower_bin; i <= upper_bin; ++i)
    {
        // Modify both positive and negative frequency components
        // (FFTs are symmetrical!)
        ModifyMagnitude(fft_output[i], multiplier);
        ModifyMagnitude(fft_output[fft_output.size() - i - 1], multiplier);
    }
}

void EqualiserEffect::ModifyMagnitude(
    std::complex<float>& number,
    const float multiplier
) const
{
    // Convert complex value to polar form
    float magnitude = std::abs(number);
    float phase = std::arg(number);

    // Remove bass
    magnitude *= multiplier;

    // Convert back to Cartesian form
    number = std::polar(magnitude, phase);
}

std::string EqualiserEffect::GetName() const
{
    return "Equaliser";
}
