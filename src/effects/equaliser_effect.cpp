#include "effects/equaliser_effect.h"
#include "effects/fft.h"

EqualiserEffect::EqualiserEffect()
{
    properties["lower frequency"] = Property(20.0f, 0.0f, 20000.0f);
    properties["upper frequency"] = Property(2000.0f, 0.0f, 20000.0f);
    properties["multiplier"] = Property(0.0f, 0.0f, 2.0f);
}

void EqualiserEffect::ApplyEffect(Packet& packet)
{
    // Abort if we're too early or too late
    if (packet.previous_samples.size() != packet.current_samples.size() ||
        packet.current_samples.size() != packet.next_samples.size())
        return;

    std::array<std::vector<float>, 3> windows = CreateWindows(packet);

    // Modify three overlapping windows separately
    for (auto& window : windows)
        ModifyWindow(window, packet.frequency);

    // Clear current samples and replace with sum of all windows for this time frame
    // This can be thought of as two overlapping "versions": the "previous" and "next"
    // windows combine to form one version of events, and the "current" window forms
    // the other.
    auto& current = packet.current_samples;
    const size_t half_size = current.size() / 2;

    packet.current_samples.clear();
    packet.current_samples.insert(current.end(), windows[0].begin() + half_size, windows[0].end());
    packet.current_samples.insert(current.end(), windows[2].begin(), windows[2].begin() + half_size);

    for (size_t i = 0; i < current.size(); ++i)
        current[i] += windows[1][i];

    // Samples will each have been "added to" multiple times so we must reduce the result
    for (float& sample : current)
        sample /= 4.0f;
}

std::array<std::vector<float>, 3> EqualiserEffect::CreateWindows(Packet& packet) const
{
    /*
        We want three "windows":
        - The last half of the previous samples plus the first half of the current
        - The entirety of the current samples
        - The last half of the current samples plus the first half of the next samples
    */

    const std::vector<float>& prev = packet.previous_samples;
    const std::vector<float>& curr = packet.current_samples;
    const std::vector<float>& next = packet.next_samples;

    const size_t half_size = packet.current_samples.size()  / 2;

    // The last of the previous samples plus the first half of the current
    std::vector<float> first_window;
    first_window.insert(first_window.begin(), prev.begin() + half_size, prev.end());
    first_window.insert(first_window.end(), curr.begin(), curr.begin() + half_size);

    // The entirety of the current samples
    std::vector<float> second_window;
    second_window.insert(second_window.begin(), curr.begin(), curr.end());

    // The last half of the current samples plus the first half of the next samples
    std::vector<float> third_window;
    third_window.insert(third_window.begin(), curr.begin() + half_size, curr.end());
    third_window.insert(third_window.end(), next.begin(), next.begin() + half_size);

    return {
        first_window,
        second_window,
        third_window
    };
}

void EqualiserEffect::ModifyWindow(std::vector<float>& window, const float frequency) const
{
    // Perform FFT to convert to frequency domain
    FastFourierTransform fft(window, std::nullopt);
    std::vector<std::complex<float>>& fft_output = fft.output;

    ModifyFrequencies(fft_output, frequency);

    // Perform IFFT to convert back to time domain
    InverseFourierTransform inverse(fft_output);
    std::vector<float> scaled_real_components;
    scaled_real_components.reserve(window.size());
    for (const auto& c : inverse.output)
        scaled_real_components.emplace_back(
            c.real() / (float)inverse.output.size()
        );

    window = scaled_real_components;
}

void EqualiserEffect::ModifyFrequencies(
    std::vector<std::complex<float>>& fft_output,
    const int frequency
) const
{
    // Properties
    const float lower_frequency = GetProperty("lower frequency");
    const float upper_frequency = GetProperty("upper frequency");
    const float multiplier = GetProperty("multiplier");

    // Work out "bins"
    const float frequency_resolution = (float)frequency / (float)fft_output.size();
    int lower_bin = lower_frequency / frequency_resolution;
    int upper_bin = upper_frequency / frequency_resolution;

    /*
        Because of the finite bin size, the lower bin may not be 0, even if the
        frequency is. Likewise, the upper bin may not be the maximum it can be
        even if the upper frequency is 20,000 Hz! This is sometimes called
        "spectral leakage" and causes quite a bit of noise. Adding a bit of
        leeway helps fix this for when the lower frequency is 0 Hz, for example.
    */
   lower_bin = std::max(lower_bin - 1, 0);
   upper_bin = std::min(upper_bin + 1, (int)fft_output.size() - 1);

    for (int i = lower_bin; i <= upper_bin; ++i)
    {
        // Modify both positive and negative frequency components
        // (FFTs are symmetrical!)
        ModifyMagnitude(fft_output[i], multiplier);
        ModifyMagnitude(fft_output[fft_output.size() - i - 1], multiplier);
    }
}

void EqualiserEffect::ModifyMagnitude(
    std::complex<float>& number,
    const float multiplier
) const
{
    // Convert complex value to polar form
    float magnitude = std::abs(number);
    float phase = std::arg(number);

    // Remove bass
    magnitude *= multiplier;

    // Convert back to Cartesian form
    number = std::polar(magnitude, phase);
}

std::string EqualiserEffect::GetName() const
{
    return "Equaliser";
}
