#include "play_window.h"

constexpr int visualiser_bar_width = 1;

PlayWindow::PlayWindow(wxWindow* parent, const Playlist& playlist) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor"), playlist(playlist)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

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
        progress_bar->SetValue(progress * progress_bar->GetMax());
        (*audio_stream)->SetProgress(progress);
    });

    // Buttons
    previous_button = new wxButton(this, wxID_ANY, "Previous");
    pause_button = new wxButton(this, wxID_ANY, "Pause");
    next_button = new wxButton(this, wxID_ANY, "Next");
    previous_button->Bind(wxEVT_BUTTON, &PlayWindow::OnPrevious, this);
    pause_button->Bind(wxEVT_BUTTON, &PlayWindow::OnPause, this);
    next_button->Bind(wxEVT_BUTTON, &PlayWindow::OnNext, this);

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
    SetMaxSize({ 800, 300 });

    StartPlayback();
}

void PlayWindow::StartPlayback()
{
    // Delete audio resources if they already exist (i.e. when re-calling function)
    if (audio_file.has_value() && audio_stream.has_value())
    {
        // We cannot simply delete the audio stream because this method may
        // have gotten called from a lambda called by the audio stream itself!
        // Instead, we must simply create a new one, then delete the old one
        // when the new one starts to play. Since an audio stream may be in
        // the process of reading from an audio file, we must be careful about
        // the lifetime of this too!
        old_audio_stream = audio_stream;
        old_audio_file = audio_file;
    }

    // Create audio file and corresponding audio stream
    audio_file.emplace(new AudioFile(playlist.Items()[current_song]));
    audio_stream.emplace(new AudioStream(*audio_file));

    (*audio_stream)->SetProgressChangedCallback([&](float progress, uint8_t* buffer, int length) {
        OnAudioStreamUpdated(progress, buffer, length);
    });

    (*audio_stream)->Play();
}

void PlayWindow::OnAudioStreamUpdated(float progress, uint8_t* buffer, int length)
{
    // Delete previous audio resources if need be (see above)
    if (old_audio_stream.has_value() && old_audio_file.has_value())
    {
        delete *old_audio_stream;
        old_audio_stream.reset();
        delete *old_audio_file;
        old_audio_file.reset();
    }

    // Update UI
    progress_bar->SetValue(progress * progress_bar->GetMax());
    UpdateVisualiserData(progress, buffer, length);

    // If song has just finished, go to the next one
    if (progress == 1.0f)
    {
        wxCommandEvent event = {};
        OnNext(event);
    }
}

void PlayWindow::UpdateVisualiserData(float progress, uint8_t* buffer, int length)
{
    AudioFile* file = *audio_file;

    // Convert from 16-bit buffer to vector of floats
    std::vector<float> audio;
    for (int i = 0; i < length / 2; ++i)
        audio.emplace_back(
            float(*((uint16_t*)buffer + i)) / (float)file->MaxSampleValue()
        );

    // Perform FFT and trigger redraw of visualiser panel
    const int n_buckets = visualiser_panel->GetSize().x / visualiser_bar_width;
    FastFourierTransform fft(audio, file->GetFrequency(), n_buckets);
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

    // Background - can be drawn on Linux without issues, but Windows has
    // double buffering issues. Luckily, on Windows we can just avoid drawing
    // the background alltogether (it will hence end up white and look
    // native anyway).
#ifndef WIN32
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(0, 0, width, height);
#endif

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

void PlayWindow::OnPrevious(wxCommandEvent& event)
{
    if (current_song == 0)
        current_song = playlist.Items().size();
    current_song--;

    StartPlayback();
}

void PlayWindow::OnPause(wxCommandEvent& event)
{
    if ((*audio_stream)->IsPlaying())
    {
        (*audio_stream)->Pause();
        pause_button->SetLabelText("Play");
    }
    else
    {
        (*audio_stream)->Play();
        pause_button->SetLabelText("Pause");
    }
}

void PlayWindow::OnNext(wxCommandEvent& event)
{
    current_song++;
    if (current_song >= playlist.Items().size())
        current_song = 0;

    StartPlayback();
}

PlayWindow::~PlayWindow()
{
    if (audio_stream.has_value())
        delete *audio_stream;
}