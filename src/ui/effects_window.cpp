#include "ui/effects_window.h"
#include "effects/volume_effect.h"

EffectsWindow::EffectsWindow(wxWindow* parent, AtomicLinkedList<AudioEffect>* effects) :
    wxFrame(parent, wxID_ANY, "Effects"), effects(effects)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    (new wxButton(this, wxID_ANY, "Quiet la!"))->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) {
        this->effects->Add(new VolumeEffect());
    });
}