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

private:
    void OnAudioStreamUpdated(float progress, uint8_t* buffer, int length);
    void PaintVisualiserPanel(const wxPaintEvent& event);

    wxPanel* visualiser_panel;
    wxSlider* progress_bar;

    const Playlist& playlist;
    std::optional<AudioFile> audio_file = {};
    std::optional<AudioStream> audio_stream = {};
    std::vector<FastFourierTransform::FrequencyRange> audio_frequencies;
};
