#pragma once
#include "effects/property.h"

struct FloatingPointProperty: Property
{
    float value = 0.0f;
    float minimum = 0.0f;
    float maximum = 1.0f;

    FloatingPointProperty() {}
    ~FloatingPointProperty() {}

    FloatingPointProperty(float default_value, float min, float max)
    {
        value = default_value;
        minimum = min;
        maximum = max;
    }

    void* GetValue() const override
    {
        return (void*)&value;
    }

    float GetSmallestUnit() const
    {
        return (maximum - minimum) / 1000.0f;
    }

    wxSlider* CreateSliderForGUI(wxWindow* parent) const override
    {
        const float smallest_unit = GetSmallestUnit();

        // Divide by smallest unit to make the slider "snap" to increments
        return new wxSlider(
            parent,
            wxID_ANY,
            value / smallest_unit,
            minimum / smallest_unit,
            maximum / smallest_unit
        );
    }

    void SetValueFromSlider(const wxSlider* slider) override
    {
        value = slider->GetValue() * GetSmallestUnit();
    }

    std::string ToString() const override
    {
        // Format to two decimal places
        return std::format("{:.2f}", value);
    }
};
