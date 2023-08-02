#include "effects/bass_effect.h"
#include "effects/fft.h"
#define _USE_MATH_DEFINES
#include <cmath>

void BassEffect::ApplyEffect(
    std::vector<float>& previous_samples,
    std::vector<float>& current_samples,
    std::vector<float>& next_samples,
    const int frequency
)
{
    /*
        In order to modify the bass without any noise artifacts,
        the FFT for the current audio must taken into account both the audio
        that comes before and the audio that comes after. If playback has
        only just started or is about to end, this will not be possible.
    */
    if (previous_samples.size() != current_samples.size() ||
        current_samples.size() != next_samples.size())
        return;

    /*
        We want three "windows". The first will be the last half of the previous
        samples plus the first half of the current samples. The second will be
        the entirety of the current samples. The last will be the last half of
        the current samples plus the first half of the next samples.
    */
    const size_t half_size = current_samples.size()  / 2;
    std::vector<float> first_window;
    first_window.insert(first_window.begin(), previous_samples.begin() + half_size, previous_samples.end());
    first_window.insert(first_window.end(), current_samples.begin(), current_samples.begin() + half_size);
    std::vector<float> second_window;
    second_window.insert(second_window.begin(), current_samples.begin(), current_samples.end());
    std::vector<float> third_window;
    third_window.insert(third_window.begin(), current_samples.begin() + half_size, current_samples.end());
    third_window.insert(third_window.end(), next_samples.begin(), next_samples.begin() + half_size);

    // Remove current samples
    for (float& sample : current_samples)
        sample = 0.0f;

    for (size_t i = 0; i < 3; ++i)
    {
        std::vector<float> window;
        if (i == 0)
            window = std::move(first_window);
        else if (i == 1)
            window = std::move(second_window);
        else
            window = std::move(third_window);

        // Perform FFT to convert to frequency domain
        FastFourierTransform fft(window, std::nullopt);
        std::vector<std::complex<float>>& fft_output = fft.output;

        PerformBassBoost(fft_output, frequency);

        // Perform IFFT to convert back to time domain
        InverseFourierTransform inverse(fft_output);
        std::vector<float> scaled_real_components;
        scaled_real_components.reserve(window.size());
        for (const auto& c : inverse.output)
            scaled_real_components.emplace_back(
                c.real() / (float)inverse.output.size()
            );

        // Write to current output if the window includes it
        const size_t size = scaled_real_components.size();
        for (size_t j = 0; j < size; ++j)
        {
            if (i == 0 && j > size / 2)
                current_samples[j - size / 2] += scaled_real_components[j];
            else if (i == 1)
                current_samples[j] += scaled_real_components[j];
            else if (i == 2 && j < size / 2)
                current_samples[j + size / 2] += scaled_real_components[j];
        }
    }

    // Samples will each have been "added to" twice so we must halve the result
    for (float& sample : current_samples)
        sample /= 2.0f;
}

void BassEffect::PerformBassBoost(
    std::vector<std::complex<float>>& fft_output,
    const int frequency
)
{
    const float lower_frequency = 20.0f;
    const float upper_frequency = 250.0f;
    const float frequency_resolution = (float)frequency / (float)fft_output.size();

    // Work out "bins"
    const int lower_bin = lower_frequency / frequency_resolution;
    const int upper_bin = upper_frequency / frequency_resolution;

    for (int i = lower_bin; i <= upper_bin; ++i)
    {
        // Convert complex value to polar form
        float magnitude = std::abs(fft_output[i]);
        float phase = std::arg(fft_output[i]);

        // Apply bass effect to the magnitude
        magnitude *= 0.01f;

        // Convert back to Cartesian form
        fft_output[i] = std::polar(magnitude, phase);
    }
}

std::string BassEffect::GetName() const
{
    return "Bass boost";
}

std::vector<std::string> BassEffect::GetPropertyNames() const
{
    return { "Minimum frequency", "Maximum frequency", "Amount" };
}
