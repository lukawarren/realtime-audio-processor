#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

class AudioEffect
{
public:

    struct Packet
    {
        std::vector<float>& previous_samples;
        std::vector<float>& current_samples;
        std::vector<float>& next_samples;
        const int frequency;
    };

    virtual void ApplyEffect(Packet& packet) = 0;

    virtual ~AudioEffect() {}

    virtual std::string GetName() const = 0;

    virtual std::vector<std::string> GetPropertyNames() const
    {
        return {};
    }

    std::unordered_map<std::string, float> properties = {};
};
