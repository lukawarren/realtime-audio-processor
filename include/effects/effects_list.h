#pragma once
#include <vector>
#include <functional>
#include "effects/volume_effect.h"
#include "effects/echo_effect.h"
#include "effects/bass_effect.h"
#include "effects/noise_effect.h"

/*
    Contains a list of all effects, their names, and a function to make them
*/

typedef std::pair<std::string, std::function<AudioEffect*()>> EffectListEntry;

static std::vector<EffectListEntry> EFFECTS_LIST =
{
    { VolumeEffect().GetName(), []() { return new VolumeEffect(); } },
    { EchoEffect().GetName(), []() { return new EchoEffect(); } },
    { BassEffect().GetName(), []() { return new BassEffect(); } },
    { NoiseEffect().GetName(), []() { return new NoiseEffect(); } }
};
