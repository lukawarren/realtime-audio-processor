#include "effects/bass_effect.h"
#include "effects/fft.h"
#define _USE_MATH_DEFINES
#include <cmath>

void BassEffect::ApplyEffect(Packet& packet)
{
    /*
        In order to modify the bass without any noise artifacts,
        the FFT for the current audio must taken into account both the audio
        that comes before and the audio that comes after. If playback has
        only just started or is about to end, this will not be possible.
    */
    if (packet.previous_samples.size() != packet.current_samples.size() ||
        packet.current_samples.size() != packet.next_samples.size())
        return;

    for (int i = 0; i < 2; ++i)
    {
        std::array<std::vector<float>, 3> windows = CreateWindows(packet);

        // Remove current samples
        for (float& sample : packet.current_samples)
            sample = 0.0f;

        for (size_t i = 0; i < 3; ++i)
        {
            // Perform FFT to convert to frequency domain
            FastFourierTransform fft(windows[i], std::nullopt);
            std::vector<std::complex<float>>& fft_output = fft.output;

            PerformBassBoost(fft_output, packet.frequency);

            // Perform IFFT to convert back to time domain
            InverseFourierTransform inverse(fft_output);
            std::vector<float> scaled_real_components;
            scaled_real_components.reserve(windows[i].size());
            for (const auto& c : inverse.output)
                scaled_real_components.emplace_back(
                    c.real() / (float)inverse.output.size()
                );

            // Write to current output
            const size_t half_size = scaled_real_components.size() / 2;
            for (size_t j = 0; j < half_size * 2; ++j)
            {
                // First window
                if (i == 0 && j > half_size)
                    packet.current_samples[j - half_size] += scaled_real_components[j];

                // Second window
                else if (i == 1)
                    packet.current_samples[j] += scaled_real_components[j];

                // Third window
                else if (i == 2 && j < half_size)
                    packet.current_samples[j + half_size] += scaled_real_components[j];
            }
        }

        // Samples will each have been "added to" multiple times so we must reduce the result
        for (float& sample : packet.current_samples)
            sample /= 4.0f;
    }
}

std::array<std::vector<float>, 3> BassEffect::CreateWindows(Packet& packet) const
{
    /*
        We want three "windows":
        - The last half of the previous samples plus the first half of the current
        - The entirety of the current samples
        - The last half of the current samples plus the first half of the next samples
    */

    const std::vector<float>& prev = packet.previous_samples;
    const std::vector<float>& curr = packet.current_samples;
    const std::vector<float>& next = packet.next_samples;

    const size_t half_size = packet.current_samples.size()  / 2;

    // The last of the previous samples plus the first half of the current
    std::vector<float> first_window;
    first_window.insert(first_window.begin(), prev.begin() + half_size, prev.end());
    first_window.insert(first_window.end(), curr.begin(), curr.begin() + half_size);

    // The entirety of the current samples
    std::vector<float> second_window;
    second_window.insert(second_window.begin(), curr.begin(), curr.end());

    // The last half of the current samples plus the first half of the next samples
    std::vector<float> third_window;
    third_window.insert(third_window.begin(), curr.begin() + half_size, curr.end());
    third_window.insert(third_window.end(), next.begin(), next.begin() + half_size);

    return {
        first_window,
        second_window,
        third_window
    };
}

void BassEffect::PerformBassBoost(
    std::vector<std::complex<float>>& fft_output,
    const int frequency
) const
{
    const float lower_frequency = 20.0f;
    const float upper_frequency = 500.0f;
    const float frequency_resolution = (float)frequency / (float)fft_output.size();

    // Work out "bins"
    const int lower_bin = lower_frequency / frequency_resolution;
    const int upper_bin = upper_frequency / frequency_resolution;

    for (int i = lower_bin; i <= upper_bin; ++i)
    {
        // Convert complex value to polar form
        float magnitude = std::abs(fft_output[i]);
        float phase = std::arg(fft_output[i]);

        // Remove bass
        magnitude = 0.0f;

        // Convert back to Cartesian form
        fft_output[i] = std::polar(magnitude, phase);
    }
}

std::string BassEffect::GetName() const
{
    return "Bass reduction";
}

std::vector<std::string> BassEffect::GetPropertyNames() const
{
    return { "Minimum frequency", "Maximum frequency", "Iterations" };
}
