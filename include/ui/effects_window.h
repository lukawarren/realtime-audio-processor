#pragma once
#include <wx/wx.h>
#include <optional>
#include <data/atomic_linked_list.h>
#include <effects/audio_effect.h>

class EffectsWindow : public wxFrame
{
public:
    EffectsWindow(wxWindow* parent, AtomicLinkedList<AudioEffect>* effects);

private:
    AtomicLinkedList<AudioEffect>* effects;
};
