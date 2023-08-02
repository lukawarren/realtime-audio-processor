#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

class AudioEffect
{
public:
    virtual ~AudioEffect() {}

    virtual void ApplyEffect(
        std::vector<float>& previous_samples,
        std::vector<float>& current_samples,
        std::vector<float>& next_samples,
        const int frequency
    ) = 0;

    virtual std::string GetName() const = 0;

    virtual std::vector<std::string> GetPropertyNames() const
    {
        return {};
    }

    std::unordered_map<std::string, float> properties = {};
};
