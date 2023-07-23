#include "fft.h"
#include <complex>

constexpr int minimum_human_frequency = 20;
constexpr int maximum_human_frequency = 20000;
constexpr int n_buckets = 100;

FastFourierTransform::FastFourierTransform(
    const std::vector<float>& samples,
    const float frequency
)
{
    // Convert samples to complex numbers
    std::vector<std::complex<float>> complex_samples;
    complex_samples.reserve(samples.size());
    for (const float sample : samples)
        complex_samples.emplace_back(std::complex<float> {
            sample,
            0.0f
        });

    // Perform FFT
    DoFFT(complex_samples);

    // Process data
    grouped_frequencies = GroupFrequencies(complex_samples, frequency);
}

void FastFourierTransform::DoFFT(std::vector<std::complex<float>>& input)
{
    const int N = input.size();
    if (N <= 1)
        return;

    std::vector<std::complex<float>> even(N / 2);
    std::vector<std::complex<float>> odd(N / 2);

    // Separate the input into even and odd indexes
    for (int i = 0; i < N / 2; ++i) {
        even[i] = input[2 * i];
        odd[i] = input[2 * i + 1];
    }

    // Recursive call for even and odd parts
    DoFFT(even);
    DoFFT(odd);

    // Compute the FFT
    for (int k = 0; k < N / 2; ++k)
    {
        std::complex<float> t = std::polar(1.0f, -2.0f * M_PIf * (float)k / (float)N) * odd[k];
        input[k] = even[k] + t;
        input[k + N / 2] = even[k] - t;
    }
}

std::vector<FastFourierTransform::FrequencyRange> FastFourierTransform::GroupFrequencies(
    const std::vector<std::complex<float>>& fft,
    const float frequency
)
{
    // Work out frequency resolution
    const size_t n_samples = fft.size();
    const float frequency_resolution = frequency / (float)n_samples;

    // Size of "buckets" (groups) - result is halved as we only take half the FFT (see below)
    const int bucket_size = n_samples / n_buckets / 2;

    // As long as all the input numbers lie strictly on the real axis (i.e. have
    // no imaginary component, as does befit audio), the FFT will be symmetrical.
    // Hence we need only look at the first half of the data! :)
    std::vector<float> buckets(n_buckets, 0.0f);
    for (size_t i = 0; i < n_samples / 2; ++i)
    {
        const float frequency = i * frequency_resolution;
        if (frequency >= minimum_human_frequency &&
            frequency <= maximum_human_frequency)
        {
            // Append to relevant "bucket"
            const int index =
                (frequency - minimum_human_frequency) /
                (maximum_human_frequency - minimum_human_frequency)
                * n_buckets;

            // Sample FFT and use complex magnitude as y-axis
            buckets[index] += std::abs(fft[i]);
        }
    }

    // Append ranges to data
    std::vector<FrequencyRange> ranges;
    ranges.reserve(buckets.size());
    for (size_t i = 0; i < buckets.size(); ++i)
    {
        int lower_freq = minimum_human_frequency + (i + 0) * bucket_size * frequency_resolution;
        int upper_freq = minimum_human_frequency + (i + 1) * bucket_size * frequency_resolution;
        ranges.emplace_back(FrequencyRange {
            .min_frequency = lower_freq,
            .max_frequency = upper_freq,
            .magnitude = buckets[i]
        });
    }

    return ranges;
}
