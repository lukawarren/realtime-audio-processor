#pragma once
#include <wx/wx.h>
#include "effects/audio_effect.h"
#include "ui/popup_window.h"

class PropertiesWindow : public PopupWindow
{
public:
    PropertiesWindow(wxWindow* parent, AudioEffect* effect);

private:
    AudioEffect* effect;
};
