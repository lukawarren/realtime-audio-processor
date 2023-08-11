#pragma once
#include <wx/wx.h>
#include <functional>
#include "ui/popup_window.h"
#include "io/playlist.h"

class SongWindow : public PopupWindow
{
public:
    SongWindow(
        wxWindow* parent,
        const Playlist& playlist,
        std::function<void(size_t)> on_song_selected
    );

private:
    wxListBox* list;
    std::function<void(size_t)> on_song_selected;
};
