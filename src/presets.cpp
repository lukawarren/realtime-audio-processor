#include "presets.h"
#include "effects/effects_list.h"

void SpaciousRoom(AtomicLinkedList<AudioEffect>* effects)
{
    // Bass (high pitched sounds are more likely to reflect off a
    // surface, so reduce low frequencies accordingly as if we're
    // hearing mostly only the ehco)
    auto* equaliser = new EqualiserEffect();
    equaliser->SetProperty<float>("upper frequency", 240.0f);
    equaliser->SetProperty<float>("lower frequency", 20.0f);
    equaliser->SetProperty<float>("multiplier", 0.0f);
    effects->Add(equaliser);

    // Echo
    effects->Add(new EchoEffect());
}

void FarAwayRoom(AtomicLinkedList<AudioEffect>* effects)
{
    // "Smudge" frequency space by "incorrectly" applying equaliser
    // (makes audio sound very echo-ey without adding delay)
    auto* equaliser = new EqualiserEffect();
    equaliser->SetProperty<float>("upper frequency", 20000.0f);
    equaliser->SetProperty<float>("lower frequency", 120.0f);
    equaliser->SetProperty<float>("multiplier", 0.0f);
    effects->Add(equaliser);

    // Echo
    effects->Add(new EchoEffect());
}

void LoFi(AtomicLinkedList<AudioEffect>* effects)
{
    // Muffle sound by suppressing frequencies
    auto* equaliser = new EqualiserEffect();
    equaliser->SetProperty<float>("upper frequency", 20000.0f);
    equaliser->SetProperty<float>("lower frequency", 250.0f);
    equaliser->SetProperty<float>("multiplier", 0.1f);
    effects->Add(equaliser);

    // Calming noise
    auto* noise = new NoiseEffect();
    noise->SetProperty<float>("intensity", 1.0f);
    effects->Add(noise);

    // Reduce volume
    auto* volume = new VolumeEffect();
    volume->SetProperty<float>("volume", 0.5f);
    effects->Add(volume);
}

void RemoveFrequencies(AtomicLinkedList<AudioEffect>* effects, float lower, float upper)
{
    auto* equaliser = new EqualiserEffect();
    equaliser->SetProperty<float>("upper frequency", upper);
    equaliser->SetProperty<float>("lower frequency", lower);
    equaliser->SetProperty<float>("multiplier", 0.0f);
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
    noise->SetProperty<float>("intensity", 2.0f);
    effects->Add(noise);

    // Very small amount of echo to simulate "tinny" sound
    auto* echo = new EchoEffect();
    echo->SetProperty<float>("fall-off", 1.3f);
    echo->SetProperty<int>("delay samples", 4096);
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
