#pragma once
#include <cstdint>
#include <string>
#include <format>
#include <wx/wx.h>

struct Property
{
    // Need to return void* as we don't know the type
    virtual void* GetValue() const = 0;

    // Interface with GUI
    virtual wxSlider* CreateSliderForGUI(wxWindow* parent) const = 0;
    virtual void SetValueFromSlider(const wxSlider* slider) = 0;
    virtual std::string ToString() const = 0;

    virtual ~Property() {};
};
