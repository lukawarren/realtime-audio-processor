#pragma once
#include <wx/wx.h>
#include "effects/audio_effect.h"
#include "data/atomic_linked_list.h"
#include "ui/popup_window.h"

class EffectsWindow : public PopupWindow
{
public:
    EffectsWindow(wxWindow* parent, const AtomicLinkedList<AudioEffect>* effects);
};
