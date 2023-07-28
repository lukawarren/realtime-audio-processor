#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

class AudioEffect
{
public:
    virtual ~AudioEffect() {}

    virtual void ApplyEffect(int16_t* buffer, int length) const = 0;

    virtual std::string GetName() const = 0;

    virtual std::vector<std::string> GetPropertyNames() const
    {
        return {};
    }

    std::unordered_map<std::string, float> properties = {};
};
