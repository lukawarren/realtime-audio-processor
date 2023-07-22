#include "play_window.h"

PlayWindow::PlayWindow(wxWindow* parent, const Playlist& playlist) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor")
{
    new wxButton(this, wxID_ANY, "bla");
}