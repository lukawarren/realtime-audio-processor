#include <iostream>
#include <complex>
#include <vector>

int main()
{
    for (int j = 0; j < 3; ++j)
    {
        int p = rand() % 5;
        std::vector<float> samples;
        for (int i = 0; i < p; ++i)
            samples.push_back((float)rand() / (float)RAND_MAX);

        std::vector<std::complex<float>> complex_samples;
        complex_samples.reserve(samples.size());
        for (const float sample : samples)
            complex_samples.emplace_back(std::complex<float> {
                sample,
                0.0f
            });

        // Print float samples
        std::cout << "Input: ";
        for (const float sample : samples)
            std::cout << sample << ", ";
        std::cout << std::endl;

        // Print complex
        std::cout << "Output: ";
        for (const std::complex<float>& sample : complex_samples)
            std::cout << sample << ", ";
        std::cout << std::endl;
        std::cout << std::endl;
    }
}