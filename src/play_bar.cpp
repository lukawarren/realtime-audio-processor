#include "play_bar.h"

PlayBar::PlayBar(wxWindow* parent) : wxPanel(parent)
{
    progress_bar = new wxGauge(this, wxID_ANY, 100000);
    progress_bar->SetValue(0);

    // Playback controls
    auto* rewind_button = new wxButton(this, wxID_ANY, "Rewind");
    auto* pause_button = new wxButton(this, wxID_ANY, "Play");
    auto* forward_button = new wxButton(this, wxID_ANY, "Forward");

    const int margin = FromDIP(10);

    wxBoxSizer* horizontal_sizer = new wxBoxSizer(wxHORIZONTAL);
    horizontal_sizer->Add(rewind_button, 0, wxALIGN_CENTER | wxALL, margin);
    horizontal_sizer->Add(pause_button, 0, wxALIGN_CENTER | wxALL, margin);
    horizontal_sizer->Add(forward_button, 0, wxALIGN_CENTER | wxALL, margin);

    wxBoxSizer* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    vertical_sizer->Add(progress_bar, 1, wxEXPAND | wxALL, margin);
    vertical_sizer->Add(horizontal_sizer, 0, wxALIGN_CENTER);
    SetSizerAndFit(vertical_sizer);
}

void PlayBar::SetPlaybackProgress(const float progress)
{
    progress_bar->SetValue(progress * progress_bar->GetRange());
}