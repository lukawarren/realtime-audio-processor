#include "ui/properties_window.h"

PropertiesWindow::PropertiesWindow(wxWindow* parent, AudioEffect* effect) :
    PopupWindow(parent, "Effect Properties"),
    effect(effect)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    (new wxButton(this, wxID_ANY, "Bob"))->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) {
        this->effect->properties.begin()->second.value = this->effect->properties.begin()->second.maximum;
    });
}
