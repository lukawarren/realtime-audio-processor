#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include "property.h"
#include "floating_point_property.h"
#include "integer_property.h"

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
    virtual std::string GetName() const = 0;

    // Need to store pointer as children could have different sizes in memory
    std::unordered_map<std::string, Property*> properties = {};

    // Need to free these pointers too
    virtual ~AudioEffect()
    {
        for (auto& property : properties)
            delete property.second;
    }

    template<typename T>
    inline T GetProperty(const std::string& name) const
    {
        // Cast raw bits into expected type
        return *(T*)properties.at(name)->GetValue();
    }

    template<typename T>
    inline void SetProperty(const std::string& name, const T value) const
    {
        T* current_value = (T*) properties.at(name)->GetValue();
        *current_value = value;
    }
};
