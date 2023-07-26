#pragma once
#include <wx/wx.h>
#include <optional>
#include "playlist.h"
#include "audio_file.h"
#include "audio_stream.h"
#include "custom_events.h"
#include "audio_visualiser.h"

class PlayWindow : public wxFrame
{
public:
    PlayWindow(wxWindow* parent, const Playlist& playlist);

private:
    void StartPlayback();

    // Events
    void OnAudioStreamUpdated(float progress, uint8_t* buffer, int length);
    void OnPrevious(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnNext(wxCommandEvent& event);

    AudioVisualiser* visualiser_panel;
    wxSlider* progress_bar;
    wxButton* previous_button;
    wxButton* pause_button;
    wxButton* next_button;

    // Playlist
    Playlist playlist;
    size_t current_song = 0;

    // Playback
    std::optional<AudioFile> audio_file = {};
    std::optional<AudioStream> audio_stream = {};
    AtomicLinkedList<AudioEffect> effects = {};
};
