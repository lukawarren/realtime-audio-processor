#include "play_window.h"
#include "audio_file.h"
#include "audio_stream.h"

PlayWindow::PlayWindow(wxWindow* parent, const Playlist& playlist) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor")
{
    new wxButton(this, wxID_ANY, "bla");

    // auto* file = new AudioFile(playlist.Items()[0]);
    // auto* stream = new AudioStream(file);
    // stream->Play();
}