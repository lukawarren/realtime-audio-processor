#pragma once
#include <wx/wx.h>
#include <optional>
#include "effects/audio_effect.h"
#include "data/atomic_linked_list.h"
#include "ui/properties_window.h"
#include "ui/popup_window.h"

class EffectsWindow : public PopupWindow
{
public:
    EffectsWindow(wxWindow* parent, AtomicLinkedList<AudioEffect>* effects);

private:
    enum class Direction { Up, Down };

    void FillList();
    void OnRemove(wxCommandEvent& e);
    void OnEdit(wxCommandEvent& e);
    void OnMove(const Direction direction);

    std::optional<ListNode<AudioEffect>*> GetSelectedEffect();
    AtomicLinkedList<AudioEffect>* effects;
    wxListBox* list_box;
};
