#pragma once
#include "effects/property.h"

struct IntegerProperty: Property
{
    int value = 0;
    int minimum = 0;
    int maximum = 255;

    IntegerProperty() {}
    ~IntegerProperty() {}

    IntegerProperty(int default_value, int min, int max)
    {
        value = default_value;
        minimum = min;
        maximum = max;
    }

    void* GetValue() const override
    {
        return (void*)&value;
    }

    wxSlider* CreateSliderForGUI(wxWindow* parent) const override
    {
        return new wxSlider(
            parent,
            wxID_ANY,
            (float) value,
            (float) minimum,
            (float) maximum
        );
    }

    void SetValueFromSlider(const wxSlider* slider) override
    {
        value = (int)slider->GetValue();
    }

    std::string ToString() const override
    {
        return std::to_string(value);
    }
};