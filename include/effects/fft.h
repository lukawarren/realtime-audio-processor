#pragma once
#include <vector>
#include <complex>
#include <optional>

/*
    As an inverse fast fourier transform is virtually identical to the normal
    version, it does in fact use the same code.
*/
class FastFourierTransform;
typedef FastFourierTransform InverseFourierTransform;

class FastFourierTransform
{
public:
    struct GroupingSettings
    {
        int n_buckets;
        int frequency;
        int minimum_audible_frequency = 20;
        int maximum_audible_frequency = 20000;
    };

    struct FrequencyRange
    {
        int min_frequency;
        int max_frequency;
        float magnitude;
    };

    // Normal fourier transform
    FastFourierTransform(
        const std::vector<float>& samples,
        std::optional<GroupingSettings> group_settings
    );

    // Inverse fourier transform - has a complex input
    FastFourierTransform(const std::vector<std::complex<float>>& samples);

    // Raw output
    std::vector<std::complex<float>> output;

    // Processed output for visualisation
    std::vector<FrequencyRange> grouped_frequencies;

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
        const GroupingSettings& group_settings
    );
    float HertzToBarkScale(const float hertz) const;
};
