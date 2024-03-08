#pragma once
#include <wx/wx.h>
#include <vector>
#include "effects/audio_effect.h"
#include "ui/popup_window.h"

class PropertiesWindow : public PopupWindow
{
public:
    PropertiesWindow(wxWindow* parent, AudioEffect* effect);

private:
    void AddProperty(const std::string& name, Property* property);
    std::string GetPropertyLabel(const std::string& name, Property* property);

    AudioEffect* effect;
    wxGridSizer* sizer;

    // Events require lifetime access to their widgets and associated
    // audio effect properties without going out of scope!
    typedef std::pair<wxStaticText*, std::string> EventInformation;
    std::vector<EventInformation> event_information;
    void OnSliderMoved(wxCommandEvent& e);
};
