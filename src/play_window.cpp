#include "play_window.h"
#include "audio_file.h"
#include "audio_stream.h"

PlayWindow::PlayWindow(wxWindow* parent, const Playlist& playlist) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor"), playlist(playlist)
{
    visualiser_panel = new wxPanel(this, wxID_ANY);
    visualiser_panel->Bind(wxEVT_PAINT, &PlayWindow::PaintVisualiserPanel, this);

    // Slider - use mouse event  (as opposed to "proper" scroll event) to avoid
    // race conditions (as it's quite possible that between clicking the slider
    // and the event being fired, the progress has already been updated by the
    // audio thread)
    progress_bar = new wxSlider(this, wxID_ANY, 0, 0, 1000000);
    progress_bar->Bind(wxEVT_LEFT_DOWN, [&](wxMouseEvent& event)
    {
        const float progress = (float)event.GetPosition().x / (float)progress_bar->GetSize().x;
        wxLogDebug("Set progress to %f", progress);
        progress_bar->SetValue(progress * progress_bar->GetMax());
    });

    // Buttons
    auto* previous_button = new wxButton(this, wxID_ANY, "Previous");
    auto* pause_button = new wxButton(this, wxID_ANY, "Pause");
    auto* next_button = new wxButton(this, wxID_ANY, "Next");

    // Button layout
    const int margin = FromDIP(10);
    auto* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(previous_button, 0, wxLEFT | wxRIGHT, margin);
    button_sizer->Add(pause_button, 0, wxLEFT | wxRIGHT, margin);
    button_sizer->Add(next_button, 0, wxLEFT | wxRIGHT, margin);

    // Overall layout
    auto* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    vertical_sizer->Add(visualiser_panel, 1, wxEXPAND);
    vertical_sizer->Add(progress_bar, 0, wxEXPAND | wxLEFT | wxRIGHT, margin);
    vertical_sizer->Add(button_sizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER, margin);
    SetSizer(vertical_sizer);
    SetMinSize({ 800, 600 });

    // audio_file = AudioFile(playlist.Items()[0]);
    audio_file.emplace(playlist.Items()[0]);
    (new AudioStream(&*audio_file))->Play();
}

void PlayWindow::PaintVisualiserPanel(const wxPaintEvent& event)
{
    const wxCoord width = visualiser_panel->GetSize().x;
    const wxCoord height = visualiser_panel->GetSize().y;

    // Background
    wxPaintDC dc(visualiser_panel);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(0, 0, width, height);
}
