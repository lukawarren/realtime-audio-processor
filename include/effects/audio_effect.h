#pragma once
#include <cstdint>
#include <string>

class AudioEffect
{
public:
    virtual ~AudioEffect() {}
    virtual void ApplyEffect(int16_t* buffer, int length) const = 0;
    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() const = 0;
};
