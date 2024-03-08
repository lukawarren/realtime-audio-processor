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
        AddProperty(name, property);

    SetSizerAndFit(sizer);
    SetMinSize({ 400, 200 });
}

void PropertiesWindow::AddProperty(const std::string& name, Property* property)
{
    // Label
    const std::string& text = GetPropertyLabel(name, property);
    auto* label = new wxStaticText(this, wxID_ANY, text);

    // Slider using polymorphic interface
    auto* slider = property->CreateSliderForGUI(this);

    // Layout
    const int margin = FromDIP(10);
    sizer->Add(label, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, margin);
    sizer->Add(slider, 0, wxRIGHT | wxEXPAND, margin);

    // Events
    slider->SetClientData((void*)event_information.size());
    event_information.emplace_back(label, name);
    slider->Bind(wxEVT_SLIDER, &PropertiesWindow::OnSliderMoved, this);
}

std::string PropertiesWindow::GetPropertyLabel(const std::string& name, Property* property)
{
    // Convert first letter of name to upper-case
    assert(name.length() >= 1);
    std::string label_text = name;
    label_text[0] = std::toupper(label_text[0]);

    // Add value itself (polymorphic interface for different types)
    label_text += ": " + property->ToString();
    return label_text;
}

void PropertiesWindow::OnSliderMoved(wxCommandEvent& e)
{
    // "Unpack" event
    wxSlider* slider = (wxSlider*) e.GetEventObject();
    EventInformation& information = event_information[(size_t)slider->GetClientData()];
    wxStaticText* label = information.first;
    std::string name = information.second;
    Property* property = effect->properties[name];

    // Set value for property itself (polymorphic interface for different types)
    property->SetValueFromSlider(slider);

    // Update text
    label->SetLabelText(GetPropertyLabel(name, property));
}
