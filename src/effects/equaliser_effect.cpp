#include "effects/equaliser_effect.h"
#include "effects/fft.h"

EqualiserEffect::EqualiserEffect()
{
    properties["lower frequency"] = Property(20.0f, 0.0f, 20000.0f);
    properties["upper frequency"] = Property(2000.0f, 0.0f, 20000.0f);
    properties["multiplier"] = Property(0.0f, 0.0f, 2.0f);
    properties["iterations"] = Property(3, 1, 5);
}

void EqualiserEffect::ApplyEffect(Packet& packet)
{
    for (int i = 0; i < properties["iterations"].value; ++i)
    {
        // Perform FFT to convert to frequency domain
        FastFourierTransform fft(packet.current_samples, std::nullopt);
        std::vector<std::complex<float>>& fft_output = fft.output;

        ModifyFrequencies(fft_output, packet.frequency);

        // Perform IFFT to convert back to time domain
        InverseFourierTransform inverse(fft_output);
        std::vector<float> scaled_real_components;
        scaled_real_components.reserve(packet.current_samples.size());
        for (const auto& c : inverse.output)
            scaled_real_components.emplace_back(
                c.real() / (float)inverse.output.size()
            );

        // Samples will each have been "added to" multiple times so we must reduce the result
        for (float& sample : scaled_real_components)
            sample /= 2.0f;

        packet.current_samples = scaled_real_components;
    }
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
    const int lower_bin = lower_frequency / frequency_resolution;
    const int upper_bin = upper_frequency / frequency_resolution;

    for (int i = lower_bin; i <= upper_bin; ++i)
    {
        // Convert complex value to polar form
        float magnitude = std::abs(fft_output[i]);
        float phase = std::arg(fft_output[i]);

        // Remove bass
        magnitude *= multiplier;

        // Convert back to Cartesian form
        fft_output[i] = std::polar(magnitude, phase);
    }
}

std::string EqualiserEffect::GetName() const
{
    return "Equaliser";
}
