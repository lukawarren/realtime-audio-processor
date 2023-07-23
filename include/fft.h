#pragma once
#include <vector>
#include <complex>

class FastFourierTransform
{
public:
    FastFourierTransform(
        const std::vector<float>& samples,
        const float frequency,
        const int n_buckets
    );

    struct FrequencyRange
    {
        int min_frequency;
        int max_frequency;
        float magnitude;
    };
    std::vector<FrequencyRange> grouped_frequencies;

private:
    // Raw FFT
    void DoFFT(std::vector<std::complex<float>>& input);

    // Data processing
    std::vector<FrequencyRange> GroupFrequencies(
        const std::vector<std::complex<float>>& fft,
        const float frequency,
        const int n_buckets
    );
};
