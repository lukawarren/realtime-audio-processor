#include "presets.h"
#include "effects/effects_list.h"

void SpaciousRoom(AtomicLinkedList<AudioEffect>* effects)
{
    // Bass (high pitched sounds are more likely to reflect off a
    // surface, so reduce low frequencies accordingly as if we're
    // hearing mostly only the ehco)
    auto* equaliser = new EqualiserEffect();
    equaliser->properties["upper frequency"].value = 250.0f;
    equaliser->properties["lower frequency"].value = 20.0f;
    equaliser->properties["multiplier"].value = 0.0f;
    effects->Add(equaliser);

    // Echo
    effects->Add(new EchoEffect());
}

void FarAwayRoom(AtomicLinkedList<AudioEffect>* effects)
{
    // "Smudge" frequency space by "incorrectly" applying equaliser
    // (makes audio sound very echo-ey without adding delay)
    auto* equaliser = new EqualiserEffect();
    equaliser->properties["upper frequency"].value = 20000.0f;
    equaliser->properties["lower frequency"].value = 20.0f;
    equaliser->properties["multiplier"].value = 0.0f;
    effects->Add(equaliser);

    // Echo
    effects->Add(new EchoEffect());
}

void LoFi(AtomicLinkedList<AudioEffect>* effects)
{
    // Muffle sound by suppressing frequencies
    auto* equaliser = new EqualiserEffect();
    equaliser->properties["lower frequency"].value = 250.0f;
    equaliser->properties["upper frequency"].value = 20000.0f;
    equaliser->properties["multiplier"].value = 0.1f;
    effects->Add(equaliser);

    // Calming noise
    auto* noise = new NoiseEffect();
    noise->properties["intensity"].value = 1.0f;
    effects->Add(noise);
}

void RemoveFrequencies(AtomicLinkedList<AudioEffect>* effects, float lower, float upper)
{
    auto* equaliser = new EqualiserEffect();
    equaliser->properties["lower frequency"].value = lower;
    equaliser->properties["upper frequency"].value = upper;
    equaliser->properties["multiplier"].value = 0.001f;
    effects->Add(equaliser);
}

void RemoveBass(AtomicLinkedList<AudioEffect>* effects)
{
    RemoveFrequencies(effects, 0.0f, 250.0f);
}

void RemoveTrebble(AtomicLinkedList<AudioEffect>* effects)
{
    RemoveFrequencies(effects, 2500.0f, 20000.0f);
}

void LowQualitySpeakers(AtomicLinkedList<AudioEffect>* effects)
{
    // Severely restrict ranges of frequencies that can be heard
    RemoveFrequencies(effects, 0.0f, 400.0f);
    RemoveFrequencies(effects, 5000.0f, 20000.0f);

    // Noise (e.g. from poor wiring)
    auto* noise = new NoiseEffect();
    noise->properties["intensity"].value = 2.0f;
    effects->Add(noise);

    // Very small amount of echo to simulate "tinny" sound
    auto* echo = new EchoEffect();
    echo->properties["fall-off"].value = 1.3f;
    echo->properties["delay samples"].value = 4096;
    effects->Add(echo);
}

std::vector<Preset> EFFECT_PRESETS =
{
    { .name = "Spacious room",          .functor = SpaciousRoom,        .speed_steps =  0 },
    { .name = "Far away room",          .functor = FarAwayRoom,         .speed_steps =  0 },
    { .name = "Lo-fi - relaxing",       .functor = LoFi,                .speed_steps = -1 },
    { .name = "Lo-fi - energy",         .functor = LoFi,                .speed_steps = +1 },
    { .name = "Remove bass",            .functor = RemoveBass,          .speed_steps =  0 },
    { .name = "Remove trebble",         .functor = RemoveTrebble,       .speed_steps =  0 },
    { .name = "Low-quality speakers",   .functor = LowQualitySpeakers,  .speed_steps =  0 },
};
