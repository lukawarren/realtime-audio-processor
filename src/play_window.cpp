#include "play_window.h"
#include <wx/dcbuffer.h>

constexpr int visualiser_bar_width = 1;

PlayWindow::PlayWindow(wxWindow* parent, const Playlist& playlist) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor"), playlist(playlist)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    // Create visualiser panel - set background style to be compatible with
    // requirements for double buffering then bind custom paint event
    visualiser_panel = new wxPanel(this, wxID_ANY);
    visualiser_panel->SetBackgroundStyle(wxBG_STYLE_PAINT);
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
        audio_stream->SetProgress(progress);
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

    // Bind event from audio thread to be received on main thread
    Bind(wxEVT_AUDIO_STREAM_UPDATED, [&](AudioStreamUpdatedEvent& event) {
        OnAudioStreamUpdated(event.progress, event.buffer, event.length);
    });

    // Create audio file and stream
    StartPlayback();
}

void PlayWindow::StartPlayback()
{
    // Delete audio resources if they already exist (i.e. when re-calling function)
    if (audio_stream.has_value()) audio_stream.reset();
    if (audio_file.has_value()) audio_file.reset();

    // Create audio file and corresponding audio stream
    audio_file.emplace(playlist.Items()[current_song]);
    audio_stream.emplace(&*audio_file);
    audio_stream->SetProgressChangedCallback([&](float progress, uint8_t* buffer, int length)
    {
        // Enqueue event on main thread
        wxQueueEvent(this, new AudioStreamUpdatedEvent(progress, buffer, length));
    });

    audio_stream->Play();
}

void PlayWindow::OnAudioStreamUpdated(float progress, uint8_t* buffer, int length)
{
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
    // Convert from 16-bit buffer to vector of floats
    std::vector<float> audio;
    for (int i = 0; i < length / 2; ++i)
        audio.emplace_back(
            float(*((uint16_t*)buffer + i)) / (float)audio_file->MaxSampleValue()
        );

    // Perform FFT
    const int n_buckets = visualiser_panel->GetSize().x / visualiser_bar_width;
    FastFourierTransform fft(audio, audio_file->GetFrequency(), n_buckets);

    // Save old results for averaging and add new one
    for (size_t i = 0; i < fft_results.size() - 1; ++i)
        fft_results[i] = fft_results[i+1];
    fft_results[fft_results.size()-1] = fft.grouped_frequencies;

    // Redraw
    visualiser_panel->Refresh();
}

void PlayWindow::PaintVisualiserPanel(const wxPaintEvent& event)
{
    const wxCoord width = visualiser_panel->GetSize().x;
    const wxCoord height = visualiser_panel->GetSize().y;

    wxAutoBufferedPaintDC context(visualiser_panel);

    // Background
    context.SetBrush(*wxBLACK_BRUSH);
    context.SetPen({ wxColour(0, 0, 0, 0), 0 });
    context.DrawRectangle(0, 0, width, height);

    // Avoid drawing before FFT data has been set by audio thread
    std::vector<FastFourierTransform::FrequencyRange>& most_recent_results = fft_results[fft_results.size()-1];
    if ((int)most_recent_results.size() < width / visualiser_bar_width)
        return;

    context.SetBrush(*wxWHITE_BRUSH);
    context.SetPen({ wxColour(255, 255, 255, 255), 0 });
    context.SetBackground(*wxWHITE_BRUSH);

    // Average FFT results over multiple "frames"
    std::vector<float> magnitudes;
    magnitudes.reserve(most_recent_results.size());
    for (size_t i = 0; i < most_recent_results.size(); ++i)
    {
        float sum = 0.0f;
        for (size_t frame =  0; frame < fft_results.size(); ++frame)
        {
            if (i < fft_results[frame].size())
                sum += fft_results[frame][i].magnitude;
        }

        magnitudes.emplace_back(sum / (float)fft_results.size());
    }

    // To work out scaling, find maximum value
    float max_magnitude = 0.0f;
    for (const auto& m : magnitudes)
        if (m > max_magnitude)
            max_magnitude = m;

    const float scale = (float)height / max_magnitude;

    // Draw bars
    for (wxCoord i = 0; i < width / visualiser_bar_width; ++i)
    {
        const int bar_height = magnitudes[i] * scale;

        context.DrawRectangle(
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
    if (audio_stream->IsPlaying())
    {
        audio_stream->Pause();
        pause_button->SetLabelText("Play");
    }
    else
    {
        audio_stream->Play();
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
