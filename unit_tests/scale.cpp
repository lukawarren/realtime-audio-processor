float HertzToBarkScale(const float hertz);

#include <iostream>
#include <string>
#include <array>
#include <cmath>

int main()
{
    const std::array<std::pair<float, float>, 5> expected_pairs = {{
        { 0.0f, 0.0f },
        { 10.0f, 0.099f },
        { 20.0f, 0.198f },
        { 100.0f, 0.987f },
        { 100000.0f, 25.727 }
    }};

    for (const auto pair : expected_pairs)
    {
        const float input = pair.first;
        const float output = HertzToBarkScale(input);
        const float error = std::abs(output - pair.second);
        std::cout << pair.first << " -> " << output << " (off by " << error << ")" << std::endl;
    }
}

float HertzToBarkScale(const float hertz)
{
    /*
        Derived from https://en.wikipedia.org/wiki/Bark_scale
    */
    return 13.0f * std::atan(0.00076f * hertz) + 3.5f * std::atan((hertz / 7500.0f) * (hertz / 7500.0f));
}
