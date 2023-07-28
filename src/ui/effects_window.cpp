#include "ui/effects_window.h"

EffectsWindow::EffectsWindow(wxWindow* parent, const AtomicLinkedList<AudioEffect>* effects) :
    PopupWindow(parent, "Modify Effects")
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    // Create list
    auto* list = new wxListBox(this, wxID_ANY);
    effects->ForEach([&](const AudioEffect* entry)
    {
        list->Insert({
            wxString::FromUTF8(entry->GetName()) },
            list->GetCount()
        );
    });

    // Buttons
    auto* remove_button = new wxButton(this, wxID_ANY, "Remove");
    auto* up_button = new wxButton(this, wxID_ANY, "Move up");
    auto* down_button = new wxButton(this, wxID_ANY, "Move down");

    // Layout
    const int margin = FromDIP(10);
    auto* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    auto* horizontal_sizer = new wxBoxSizer(wxRA_HORIZONTAL);
    horizontal_sizer->Add(remove_button);
    horizontal_sizer->Add(up_button, 0, wxLEFT | wxRIGHT, margin);
    horizontal_sizer->Add(down_button);
    vertical_sizer->Add(list, 0, wxEXPAND | wxALL, margin);
    vertical_sizer->Add(horizontal_sizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, margin);
    SetSizerAndFit(vertical_sizer);
}
