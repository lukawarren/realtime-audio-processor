#include "ui/properties_window.h"
#include <cassert>
#include <cctype>

PropertiesWindow::PropertiesWindow(wxWindow* parent, AudioEffect* effect) :
    PopupWindow(parent, "Effect Properties"),
    effect(effect)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    sizer = new wxGridSizer(2);

    for (auto& [name, property] : effect->properties)
        AddProperty(name, &property);

    SetSizerAndFit(sizer);
    SetMinSize({ 400, 200 });
}

void PropertiesWindow::AddProperty(const std::string& name, AudioEffect::Property* property)
{
    // Convert first letter of name to upper-case
    assert(name.length() >= 1);
    std::string label_text = name;
    label_text[0] = std::toupper(label_text[0]);

    // Label
    auto* label = new wxStaticText(this, wxID_ANY, label_text);

    // Slider
    auto* slider = new wxSlider(this, wxID_ANY, property->value, property->minimum, property->maximum);

    // Layout
    const int margin = FromDIP(10);
    sizer->Add(label, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, margin);
    sizer->Add(slider, 0, wxRIGHT | wxEXPAND, margin);

    // Events
    slider->SetClientData((void*)event_information.size());
    event_information.emplace_back(label, property);
    slider->Bind(wxEVT_SLIDER, &PropertiesWindow::OnSliderMoved, this);
}

void PropertiesWindow::OnSliderMoved(wxCommandEvent& e)
{
    // "Unpack" event
    wxSlider* slider = (wxSlider*) e.GetEventObject();
    EventInformation& information = event_information[(size_t)slider->GetClientData()];
    wxStaticText* label = information.first;
    AudioEffect::Property* property = information.second;

    property->value = slider->GetValue();
    label->SetLabelText(std::to_string(slider->GetValue()));
}
