#pragma once
#include <vector>
#include <complex>

class FastFourierTransform
{
public:
    FastFourierTransform(
        const std::vector<float>& samples,
        const float frequency,
        const int n_buckets,
        const int minimum_audible_frequency = 20,
        const int maximum_audible_frequency = 20000
    );

    struct FrequencyRange
    {
        int min_frequency;
        int max_frequency;
        float magnitude;
    };

    std::vector<FrequencyRange> grouped_frequencies;
    int minimum_audible_frequency;
    int maximum_audible_frequency;

private:
    // Raw FFT
    void DoFFT(std::vector<std::complex<float>>& input);

    // Data processing
    std::vector<FrequencyRange> GroupFrequencies(
        const std::vector<std::complex<float>>& fft,
        const float frequency,
        const int n_buckets
    );
    float HertzToBarkScale(const float hertz);
};
