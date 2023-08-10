#include "ui/effects_window.h"

EffectsWindow::EffectsWindow(wxWindow* parent, AtomicLinkedList<AudioEffect>* effects) :
    PopupWindow(parent, "Modify Effects"),
    effects(effects)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    // Create list
    list_box = new wxListBox(this, wxID_ANY);
    FillList();

    // Buttons
    auto* remove_button = new wxButton(this, wxID_ANY, "Remove");
    auto* edit_button = new wxButton(this, wxID_ANY, "Edit");
    auto* up_button = new wxButton(this, wxID_ANY, "Move up");
    auto* down_button = new wxButton(this, wxID_ANY, "Move down");

    // Layout
    const int margin = FromDIP(10);
    auto* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    auto* horizontal_sizer = new wxBoxSizer(wxRA_HORIZONTAL);
    horizontal_sizer->Add(remove_button);
    horizontal_sizer->Add(edit_button, 0, wxLEFT, margin);
    horizontal_sizer->Add(up_button, 0, wxLEFT | wxRIGHT, margin);
    horizontal_sizer->Add(down_button);
    vertical_sizer->Add(list_box, 1, wxEXPAND | wxALL, margin);
    vertical_sizer->Add(horizontal_sizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER, margin);

    // Events
    remove_button->Bind(wxEVT_BUTTON, &EffectsWindow::OnRemove, this);
    edit_button->Bind(wxEVT_BUTTON, &EffectsWindow::OnEdit, this);
    up_button->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) {
        OnMove(Direction::Up);
    });
    down_button->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) {
        OnMove(Direction::Down);
    });

    SetSizerAndFit(vertical_sizer);
}

void EffectsWindow::FillList()
{
    list_box->Clear();
    effects->ForEach([&](const AudioEffect* entry)
    {
        list_box->Insert({
            wxString::FromUTF8(entry->GetName()) },
            list_box->GetCount()
        );
    });
}

void EffectsWindow::OnRemove(wxCommandEvent& e)
{
    auto effect = GetSelectedEffect();
    if (effect.has_value())
        (effect.value())->Remove();

    FillList();
}

void EffectsWindow::OnEdit(wxCommandEvent& e)
{
    auto selected = GetSelectedEffect();
    if (selected.has_value())
    {
        // Check effect actually has modifiable proeprties
        AudioEffect* effect = selected.value()->GetData();
        if (effect->properties.size() == 0)
            wxMessageBox("This effect cannot be modified");
        else
            new PropertiesWindow(this, effect);
    }
}

void EffectsWindow::OnMove(const Direction direction)
{
    auto effect = GetSelectedEffect();
    const int index = list_box->GetSelection();
    if (!effect.has_value()) return;

    if (direction == Direction::Up)
    {
        // Swap with previous element but don't let the index go too low
        (effect.value())->SwapWithPrevious();
        FillList();
        list_box->Select(std::max(index - 1, 0));
    }
    else
    {
        // Swap with next element but don't let the index go too high
        (effect.value())->SwapWithNext();
        FillList();
        list_box->Select(std::min(index + 1, (int)list_box->GetCount() - 1));
    }
}

std::optional<ListNode<AudioEffect>*> EffectsWindow::GetSelectedEffect()
{
    const int index = list_box->GetSelection();
    if (index < 0) return {};
    return { effects->GetNthItem(index) };
}
