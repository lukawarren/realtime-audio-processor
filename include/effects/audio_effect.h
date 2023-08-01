#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

class AudioEffect
{
public:
    virtual ~AudioEffect() {}

    virtual void ApplyEffect(std::vector<float>& samples) = 0;

    virtual std::string GetName() const = 0;

    virtual std::vector<std::string> GetPropertyNames() const
    {
        return {};
    }

    std::unordered_map<std::string, float> properties = {};
};
