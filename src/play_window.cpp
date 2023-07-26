#include "play_window.h"

PlayWindow::PlayWindow(wxWindow* parent, const Playlist& playlist) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor"), playlist(playlist)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    // Visualiser panel
    visualiser_panel = new AudioVisualiser(this);

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
    SetMinSize({ 800, 200 });
    SetMaxSize({ 800, 500 });
    SetSize({800, 500});

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
    audio_stream.emplace(&*audio_file, &effects);
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
    visualiser_panel->FeedAudio(buffer, length, audio_file.value());

    // If song has just finished, go to the next one
    if (progress == 1.0f)
    {
        wxCommandEvent event = {};
        OnNext(event);
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
