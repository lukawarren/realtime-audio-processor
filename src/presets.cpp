#include "presets.h"
#include "effects/effects_list.h"

void SpaciousRoom(AtomicLinkedList<AudioEffect>* effects)
{
    // Bass (high pitched sounds are more likely to reflect off a
    // surface, so reduce low frequencies accordingly as if we're
    // hearing mostly only the ehco)
    auto* equaliser = new EqualiserEffect();
    equaliser->properties["upper frequency"].value = 250.0f;
    effects->Add(equaliser);

    // Echo
    effects->Add(new EchoEffect());
}

void LoFi(AtomicLinkedList<AudioEffect>* effects)
{
    // "Smooth" most frequencies (save for high vocals) so as to muffle them
    auto* equaliser = new EqualiserEffect();
    equaliser->properties["lower frequency"].value = 250.0f;
    equaliser->properties["upper frequency"].value = 8000.0f;
    equaliser->properties["multiplier"].value = 0.1f;
    effects->Add(equaliser);

    // Calming noise
    auto* noise = new NoiseEffect();
    noise->properties["intensity"].value = 5.0f;
    effects->Add(noise);

    // Reduce volume
    auto* volume = new VolumeEffect();
    volume->properties["volume"].value = 0.5f;
    effects->Add(volume);
}

void RemoveFrequencies(AtomicLinkedList<AudioEffect>* effects, float lower, float upper)
{
    // Use many iterations to really stamp it out! :)
    auto* equaliser = new EqualiserEffect();
    equaliser->properties["lower frequency"].value = lower;
    equaliser->properties["upper frequency"].value = upper;
    equaliser->properties["multiplier"].value = 0.1f;
    equaliser->properties["iterations"].value = 5;
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
    noise->properties["intensity"].value = 7.0f;
    effects->Add(noise);

    // Very small amount of echo to simulate "tinny" sound
    auto* echo = new EchoEffect();
    echo->properties["fall-off"].value = 1.3f;
    echo->properties["delay samples"].value = 4096;
    effects->Add(echo);
}

std::vector<Preset> EFFECT_PRESETS =
{
    { .name = "Spacious Room",          .functor = SpaciousRoom,        .speed_steps =  0 },
    { .name = "Lo-fi - relaxing",       .functor = LoFi,                .speed_steps = -1 },
    { .name = "Lo-fi - energy",         .functor = LoFi,                .speed_steps = +1 },
    { .name = "Remove bass",            .functor = RemoveBass,          .speed_steps =  0 },
    { .name = "Remove trebble",         .functor = RemoveTrebble,       .speed_steps =  0 },
    { .name = "Low-quality Speakers",   .functor = LowQualitySpeakers,  .speed_steps =  0 },
};
