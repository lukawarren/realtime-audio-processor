#include "play_window.h"

constexpr int visualiser_bar_width = 1;

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
    SetMinSize({ 800, 300 });

    audio_file.emplace(playlist.Items()[0]);
    audio_stream.emplace(&*audio_file);
    audio_stream->SetProgressChangedCallback([&](float progress, uint8_t* buffer, int length) {
        PlayWindow::OnAudioStreamUpdated(progress, buffer, length);
    });
    audio_stream->Play();
}

void PlayWindow::OnAudioStreamUpdated(float progress, uint8_t* buffer, int length)
{
    // Convert from 16-bit buffer to vector of floats
    std::vector<float> audio;
    for (int i = 0; i < length / 2; ++i)
        audio.emplace_back(
            float(*((uint16_t*)buffer + i)) / (float)audio_file->MaxSampleValue()
        );

    // Perform FFT and trigger redraw of visualiser panel
    const int n_buckets = visualiser_panel->GetSize().x / visualiser_bar_width;
    FastFourierTransform fft(audio, audio_file->GetFrequency(), n_buckets);
    audio_frequencies = fft.grouped_frequencies;
    visualiser_panel->Refresh();
}

void PlayWindow::PaintVisualiserPanel(const wxPaintEvent& event)
{
    const wxCoord width = visualiser_panel->GetSize().x;
    const wxCoord height = visualiser_panel->GetSize().y;

    wxPaintDC dc(visualiser_panel);

    // Blank pen to disable outlien
    wxPen pen(wxColour(0, 0, 0, 0), 0);
    dc.SetPen(pen);

    // Background
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(0, 0, width, height);

    // Avoid drawing before FFT data has been set by audio thread
    if ((int)audio_frequencies.size() < width / visualiser_bar_width)
        return;

    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetBackground(*wxWHITE_BRUSH);

    // To work out scaling, find maximum value
    float max_magnitude = 0.0f;
    for (const auto& freq : audio_frequencies)
        if (freq.magnitude > max_magnitude)
            max_magnitude = freq.magnitude;

    const float scale = (float)height / max_magnitude;

    // Draw bars
    for (wxCoord i = 0; i < width / visualiser_bar_width; ++i)
    {
        const float sample = audio_frequencies[i].magnitude;
        const int bar_height = sample * scale;
        dc.DrawRectangle(
            i * visualiser_bar_width,   // X
            height - bar_height - 1,    // Y
            visualiser_bar_width,       // Width
            bar_height                  // Height
        );
    }
}
