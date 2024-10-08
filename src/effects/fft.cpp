#define _USE_MATH_DEFINES
#include "effects/fft.h"
#include <cmath>
#include <complex>

FastFourierTransform::FastFourierTransform(
    const std::vector<float>& samples,
    std::optional<GroupingSettings> group_settings
)
{
    // "Normal" FFT
    std::vector<std::complex<float>> complex = ConvertSamplesToComplexForm(samples);
    DoFFT(complex, Mode::Normal);
    output = complex;

    // Process data for visualisation (if need be!)
    if (group_settings.has_value())
        grouped_frequencies = GroupFrequencies(complex, *group_settings);
}

InverseFourierTransform::FastFourierTransform(const std::vector<std::complex<float>>& samples)
{
    // Inverse FFT
    output = samples;
    DoFFT(output, Mode::Inverse);
}

std::vector<std::complex<float>> FastFourierTransform::ConvertSamplesToComplexForm(
    const std::vector<float>& samples
)
{
    // Verify size is a power of 2 (otherwise the Cooley–Tukey algorithm will
    // not work)
    if ((samples.size() & (samples.size() - 1)) != 0)
        throw std::runtime_error("FFT data has invalid size");

    // Convert samples to complex numbers
    std::vector<std::complex<float>> complex_samples;
    complex_samples.reserve(samples.size());
    for (const float sample : samples)
        complex_samples.emplace_back(std::complex<float> {
            sample,
            0.0f
        });

    return complex_samples;
}

void FastFourierTransform::DoFFT(std::vector<std::complex<float>>& input, const Mode mode)
{
    const int N = input.size();

    // Base case
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
    DoFFT(even, mode);
    DoFFT(odd, mode);

    // Compute the FFT
    for (int k = 0; k < N / 2; ++k)
    {
        // Inverse and normal FFTs differ in their sign (which can thought of
        // as the direction of the rotation in the complex plane)
        const float sign = (mode == Mode::Normal) ? -1.0f : 1.0f;

        std::complex<float> t = std::polar(1.0f, sign * 2.0f * (float)M_PI * (float)k / (float)N) * odd[k];
        input[k] = even[k] + t;
        input[k + N / 2] = even[k] - t;
    }
}

std::vector<FastFourierTransform::FrequencyRange> FastFourierTransform::GroupFrequencies(
    const std::vector<std::complex<float>>& fft,
    const GroupingSettings& group_settings
)
{
    // Work out frequency resolution
    const size_t n_samples = fft.size();
    const float frequency_resolution = (float)group_settings.frequency / (float)n_samples;

    // Bark scale
    const float minimum_frequency_bark = HertzToBarkScale(group_settings.minimum_audible_frequency);
    const float maximum_frequency_bark = HertzToBarkScale(group_settings.maximum_audible_frequency);
    const float bark_distance =
        (maximum_frequency_bark - minimum_frequency_bark) /
        group_settings.n_buckets;

    // As long as all the input numbers lie strictly on the real axis (i.e. have
    // no imaginary component, as does befit audio), the FFT will be symmetrical.
    // Hence we need only look at the first half of the data! :)
    std::vector<float> buckets(group_settings.n_buckets, 0.0f);
    for (size_t i = 0; i < n_samples / 2; ++i)
    {
        const float frequency = i * frequency_resolution;
        if (frequency >= group_settings.minimum_audible_frequency &&
            frequency <= group_settings.maximum_audible_frequency)
        {
            // Apply Bark scale
            const float bark_frequency = HertzToBarkScale(frequency);

            // Append to relevant "bucket"
            const int index = (bark_frequency - minimum_frequency_bark) / bark_distance;

            // Sample FFT and use complex magnitude as y-axis
            if ((size_t)index < buckets.size())
                buckets[index] += std::abs(fft[i]);
        }
    }

    // As with the X-axis above, the Y-axis should be logarithmic for audio too!
    // To scale this correctly, find the maximum magnitude.
    float max_magnitude = 0.0f;
    for (size_t i = 0; i < buckets.size(); ++i)
        if (buckets[i] > max_magnitude)
            max_magnitude = buckets[i];

    // Scale magnitudes logarithmically
    for (size_t i = 0; i < buckets.size(); ++i)
        buckets[i] = std::log10(1.0f + buckets[i] / max_magnitude);

    // Append ranges to data
    std::vector<FrequencyRange> ranges;
    ranges.reserve(buckets.size());
    for (size_t i = 0; i < buckets.size(); ++i)
    {
        float lower_freq = minimum_frequency_bark + (i + 0.0f) * bark_distance;
        float upper_freq = maximum_frequency_bark + (i + 1.0f) * bark_distance;
        lower_freq = 0.5f + 600.0f * std::sinh(lower_freq / 6.0f);
        upper_freq = 0.5f + 600.0f * std::sinh(upper_freq / 6.0f);

        ranges.emplace_back(FrequencyRange {
            .min_frequency = (int)lower_freq,
            .max_frequency = (int)upper_freq,
            .magnitude = buckets[i]
        });
    }

    return ranges;
}

float FastFourierTransform::HertzToBarkScale(const float hertz) const
{
    /*
        Derived from https://en.wikipedia.org/wiki/Bark_scale
    */
    return 13.0f * std::atan(0.00076f * hertz) + 3.5f * std::atan((hertz / 7500.0f) * (hertz / 7500.0f));
}
