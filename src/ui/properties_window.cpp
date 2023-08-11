#include "ui/properties_window.h"
#include <cassert>
#include <cctype>
#include <format>

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
    // Label
    const std::string& text = GetPropertyLabel(name, property);
    auto* label = new wxStaticText(this, wxID_ANY, text);

    // Slider
    auto* slider = new wxSlider(
        this,
        wxID_ANY,
        property->value / property->GetSmallestUnit(),
        property->minimum / property->GetSmallestUnit(),
        property->maximum / property->GetSmallestUnit()
    );

    // Layout
    const int margin = FromDIP(10);
    sizer->Add(label, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, margin);
    sizer->Add(slider, 0, wxRIGHT | wxEXPAND, margin);

    // Events
    slider->SetClientData((void*)event_information.size());
    event_information.emplace_back(label, name);
    slider->Bind(wxEVT_SLIDER, &PropertiesWindow::OnSliderMoved, this);
}

std::string PropertiesWindow::GetPropertyLabel(const std::string& name, AudioEffect::Property* property)
{
    // Convert first letter of name to upper-case
    assert(name.length() >= 1);
    std::string label_text = name;
    label_text[0] = std::toupper(label_text[0]);

    if (property->is_integer)
    {
        label_text += ": " + std::to_string((int)property->value);
    }
    else
    {
        // Append value to N decimal places
        label_text += ": " + std::format("{:.2f}", property->value);
    }

    return label_text;
}

void PropertiesWindow::OnSliderMoved(wxCommandEvent& e)
{
    // "Unpack" event
    wxSlider* slider = (wxSlider*) e.GetEventObject();
    EventInformation& information = event_information[(size_t)slider->GetClientData()];
    wxStaticText* label = information.first;
    std::string name = information.second;
    AudioEffect::Property* property = &effect->properties[name];

    // Set value for property itself
    property->value = (float)slider->GetValue() * property->GetSmallestUnit();

    // Update text
    label->SetLabelText(GetPropertyLabel(name, property));
}
