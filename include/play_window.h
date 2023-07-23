#pragma once
#include <wx/wx.h>
#include <optional>
#include "playlist.h"
#include "audio_file.h"
#include "audio_stream.h"
#include "fft.h"

class PlayWindow : public wxFrame
{
public:
    PlayWindow(wxWindow* parent, const Playlist& playlist);
    ~PlayWindow();

private:
    void StartPlayback();

    // Events
    void OnAudioStreamUpdated(float progress, uint8_t* buffer, int length);
    void UpdateVisualiserData(float progress, uint8_t* buffer, int length);
    void PaintVisualiserPanel(const wxPaintEvent& event);
    void OnPrevious(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnNext(wxCommandEvent& event);

    wxPanel* visualiser_panel;
    wxSlider* progress_bar;
    wxButton* previous_button;
    wxButton* pause_button;
    wxButton* next_button;

    Playlist playlist;
    size_t current_song = 0;
    std::optional<AudioFile*> audio_file = {};
    std::optional<AudioStream*> audio_stream = {};
    std::optional<AudioFile*> old_audio_file = {};
    std::optional<AudioStream*> old_audio_stream = {};
    std::vector<FastFourierTransform::FrequencyRange> audio_frequencies = {};
};
