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
// Make the properties window a friend class as it needs to enumerate
// over the raw properties hashmap to access the keys (i.e. the property
// names). Whilst one could instead define a public interface such as
// GetPropertyKeys(), this would result in a large number of heap allocations
// (one for a std::vector then one for each key stored as a std::string),
// which would impact negatively on performance as heap allocations are costly.
friend class PropertiesWindow;

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

    inline bool HasProperties() const
    {
        return !properties.empty();
    }

// Use protected to allow derived classes to access the properties directly,
// whilst forcing external code to use the interfaces defined above
protected:
    // Need to store pointers as children could have different sizes in memory
    std::unordered_map<std::string, Property*> properties = {};
};
