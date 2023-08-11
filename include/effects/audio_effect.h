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

    struct Property
    {
        float value = 0.0f;
        float minimum = 0.0f;
        float maximum = 1.0f;
        bool is_integer = false;

        Property() {}

        Property(float default_value, float min, float max)
        {
            value = default_value;
            minimum = min;
            maximum = max;
            is_integer = false;
        }

        Property(int default_value, int min, int max)
        {
            value = default_value;
            minimum = min;
            maximum = max;
            is_integer = true;
        }

        float GetSmallestUnit() const
        {
            if (is_integer) return 1.0f;
            else return (maximum - minimum) / 100.0f;
        }
    };

    virtual void ApplyEffect(Packet& packet) = 0;
    virtual ~AudioEffect() {}
    virtual std::string GetName() const = 0;
    std::unordered_map<std::string, Property> properties = {};

protected:
    inline float GetProperty(const std::string& name) const
    {
        return properties.at(name).value;
    }
};
