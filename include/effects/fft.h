#pragma once
#include <vector>
#include <complex>

/*
    As an inverse fast fourier transform is virtually identical to the normal
    version, it does in fact use the same code.
*/
class FastFourierTransform;
typedef FastFourierTransform InverseFourierTransform;

class FastFourierTransform
{
public:

    // Normal fourier transform
    FastFourierTransform(
        const std::vector<float>& samples,
        const float frequency,
        const int n_buckets,
        const int minimum_audible_frequency = 20,
        const int maximum_audible_frequency = 20000
    );

    // Inverse fourier transform - has a complex input
    FastFourierTransform(const std::vector<std::complex<float>>& samples);

    struct FrequencyRange
    {
        int min_frequency;
        int max_frequency;
        float magnitude;
    };

    // Processed output for visualisation
    std::vector<FrequencyRange> grouped_frequencies;
    int minimum_audible_frequency;
    int maximum_audible_frequency;

    // Raw output
    std::vector<std::complex<float>> output;

private:
    std::vector<std::complex<float>> ConvertSamplesToComplexForm(
        const std::vector<float>& samples
    );

    // Handles both normal and inverse FFTs
    enum class Mode { Normal, Inverse };
    void DoFFT(std::vector<std::complex<float>>& input, const Mode mode);

    // Data processing
    std::vector<FrequencyRange> GroupFrequencies(
        const std::vector<std::complex<float>>& fft,
        const float frequency,
        const int n_buckets
    );
    float HertzToBarkScale(const float hertz);
};
