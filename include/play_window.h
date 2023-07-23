#pragma once
#include <wx/wx.h>
#include <optional>
#include "playlist.h"
#include "audio_file.h"

class PlayWindow : public wxFrame
{
public:
    PlayWindow(wxWindow* parent, const Playlist& playlist);

private:
    void PaintVisualiserPanel(const wxPaintEvent& event);

    wxPanel* visualiser_panel;
    wxSlider* progress_bar;

    const Playlist& playlist;
    std::optional<AudioFile> audio_file = {};
};
