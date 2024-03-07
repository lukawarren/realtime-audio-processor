#include "effects/fourier_effect.h"
#include "effects/fft.h"

void FourierEffect::ApplyEffect(Packet& packet)
{
    /*
        Because we only apply the FFT effect to the current packet,
        the boundaries between adjecent packets will often suffer from
        sudden jumps in amplitude, because the FFT does not know what audio
        comes after, and what comes before. To solve this, we combine the
        current packet with the previous and next packet.

        However, this can only be done if the previous and next packets
        do indeed exist (which they won't when we're at the beginning or
        end of an audio file).
    */

   if (packet.next_samples.size() == packet.current_samples.size() &&
        packet.previous_samples.size() == packet.current_samples.size())
    {
        // Copy half of previous packet, entire current packet, and half of next packet
        std::vector<float> combined_samples;
        combined_samples.reserve(packet.current_samples.size() * 3);
        combined_samples.insert(
            combined_samples.end(),
            packet.previous_samples.begin() + packet.previous_samples.size() / 2,
            packet.previous_samples.end()
        );
        combined_samples.insert(
            combined_samples.end(),
            packet.current_samples.begin(),
            packet.current_samples.end()
        );
        combined_samples.insert(
            combined_samples.end(),
            packet.next_samples.begin(),
            packet.next_samples.begin() + packet.next_samples.size() / 2
        );

        ModifySamples(combined_samples, packet.frequency);

        // Copy only the middle part back to packet current samples
        packet.current_samples.assign(
            combined_samples.begin() + combined_samples.size() / 4,
            combined_samples.begin() + combined_samples.size() / 4 + combined_samples.size() / 2
        );

        // Do the same for previous and next samples (because later audio effects
        // may need to do the same process as us)
        packet.previous_samples.assign(
            combined_samples.begin(),
            combined_samples.begin() + combined_samples.size() / 4
        );
        packet.next_samples.assign(
            combined_samples.begin() + combined_samples.size() / 4 * 3,
            combined_samples.end()
        );
    }
    else
    {
        ModifySamples(packet.current_samples, packet.frequency);
    }
}

void FourierEffect::ModifySamples(std::vector<float>& samples, const float frequency) const
{
    // Perform FFT to convert to frequency domain
    FastFourierTransform fft(samples, std::nullopt);
    std::vector<std::complex<float>>& fft_output = fft.output;

    ModifyFrequencies(fft_output, frequency);

    // Perform IFFT to convert back to time domain
    InverseFourierTransform inverse(fft_output);
    std::vector<float> scaled_real_components;
    scaled_real_components.reserve(samples.size());
    for (const auto& c : inverse.output)
        scaled_real_components.emplace_back(
            c.real() / (float)inverse.output.size()
        );

    samples = scaled_real_components;
}
