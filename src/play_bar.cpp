#include "play_bar.h"

PlayBar::PlayBar(wxWindow* parent, const std::function<void(float)> on_progress_changed) :
    wxPanel(parent), on_progress_changed(on_progress_changed)
{
    // Playback controls
    auto* pause_button = new wxButton(this, wxID_ANY, "Play");

    // Slider - use mouse event  (as opposed to "proper" scroll event) to avoid
    // race conditions (as it's quite possible that between clicking the slider
    // and the event being fired, the progress has already been updated by the
    // audio thread)
    progress_bar = new wxSlider(this, wxID_ANY, 0, 0, 1000000);
    progress_bar->SetValue(0);
    progress_bar->Bind(wxEVT_LEFT_DOWN, [&](wxMouseEvent& event)
    {
        const float progress = (float)event.GetPosition().x / (float)progress_bar->GetSize().x;
        this->on_progress_changed(progress);
    });

    const int margin = FromDIP(10);

    wxBoxSizer* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    vertical_sizer->Add(progress_bar, 1, wxEXPAND | wxALL, margin);
    vertical_sizer->Add(pause_button, 0, wxALIGN_CENTER | wxALL, margin);
    SetSizerAndFit(vertical_sizer);
}

void PlayBar::SetPlaybackProgress(const float progress)
{
    progress_bar->SetValue(progress * progress_bar->GetMax());
}
