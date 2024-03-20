#include <complex>
void ModifyMagnitude(std::complex<float>& number, const float multiplier);
#include <iostream>
#include <complex>

int main()
{
    for (int j = 0; j < 3; ++j)
    {
        const float real_component = (float)rand() / (float)RAND_MAX;
        const float imaginary_component = (float)rand() / (float)RAND_MAX;
        std::complex<float> complex(real_component, imaginary_component);

        std::cout << "Argument before: " << std::arg(complex) << std::endl;
        std::cout << "Magnitude before: " << std::abs(complex) << std::endl;

        ModifyMagnitude(complex, 0.5f);

        std::cout << "Argument after: " << std::arg(complex) << std::endl;
        std::cout << "Magnitude after: " << std::abs(complex) << std::endl;
        std::cout << std::endl;
    }
}

void ModifyMagnitude(std::complex<float>& number, const float multiplier)
{
    // Convert complex value to polar form
    float magnitude = std::abs(number);
    float phase = std::arg(number);

    // Manipulate magnitude
    magnitude *= multiplier;

    // Convert back to Cartesian form
    number = std::polar(magnitude, phase);
}
