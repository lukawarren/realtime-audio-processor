#include "effects/equaliser_effect.h"
#include "effects/fft.h"

EqualiserEffect::EqualiserEffect()
{
    properties["lower frequency"] = new FloatingPointProperty(20.0f, 0.0f, 20000.0f);
    properties["upper frequency"] = new FloatingPointProperty(2000.0f, 0.0f, 20000.0f);
    properties["multiplier"] = new FloatingPointProperty(0.0f, 0.0f, 2.0f);
}

void EqualiserEffect::ApplyEffect(Packet& packet)
{
    /*
        Because we only apply the equaliser effect to the current packet,
        the boundaries between adjecent packets will often suffer from
        sudden jumps in amplitude, because the FFT does not know what audio
        comes after, and what comes before. To solve this, we combine the
        current packet with the previous and next packet. As a consequence,
        the "sudden jumps" are still there, but appear at the beginning of the
        previous packet and at the end of the next packet. Thus when we only
        extract the audio data for the current packet, we avoid extracting
        any "jumps".

        Note that the FFT algorithm used requires data to be a power of 2 as
        a consequence of how it divides and conquers using recursion. As a
        result, we can only take *half* of the previous and next packets,
        because 1 + 1/2 + 1/2 = 2 = 2^1. This is actually favourable as it
        reduces the number of sound packets that must be processed, and as
        the "sudden jumps" are still not present in the *current* packet, there
        is no difference in the final audio generated.

        However, this can only be done if the previous and next packets
        do indeed exist (which they won't when we're at the beginning or
        end of an audio file). We must therefore verify that the appropriate
        packets are present and of the same size as the current one (to
        preserve the power of 2 restriction).
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

void EqualiserEffect::ModifySamples(std::vector<float>& samples, const float frequency) const
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

void EqualiserEffect::ModifyFrequencies(
    std::vector<std::complex<float>>& fft_output,
    const int frequency
) const
{
    // Properties
    const float lower_frequency = GetProperty<float>("lower frequency");
    const float upper_frequency = GetProperty<float>("upper frequency");
    const float multiplier = GetProperty<float>("multiplier");

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
    /*
        We must take great care to not modify the phase of the complex audio
        value, as that'll result in all sorts of audio distortion (as the phase
        is, in essence, the time offset of the frequency). Instead, we must
        convert the complex value to polar form, manipulate the magnitude, and
        then convert back to Cartesian form.
    */

    // Convert complex value to polar form
    float magnitude = std::abs(number);
    float phase = std::arg(number);

    // Manipulate magnitude
    magnitude *= multiplier;

    // Convert back to Cartesian form
    number = std::polar(magnitude, phase);
}

std::string EqualiserEffect::GetName() const
{
    return "Equaliser";
}
