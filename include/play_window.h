#pragma once
#include <wx/wx.h>
#include "playlist.h"

class PlayWindow : public wxFrame
{
public:
    PlayWindow(wxWindow* parent, const Playlist& playlist);
};
