#pragma once
#include <vector>
#include <functional>
#include "effects/audio_effect.h"
#include "data/atomic_linked_list.h"

// A function that modifies a list of audio effects to apply a "preset"
typedef std::function<void(AtomicLinkedList<AudioEffect>*)> PresetFunctor;

struct Preset
{
    std::string name;
    PresetFunctor functor;
    int speed_steps = 0;
};

extern std::vector<Preset> EFFECT_PRESETS;
