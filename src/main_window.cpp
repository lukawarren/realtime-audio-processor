#include "main_window.h"
#include "file_browser.h"
#include "play_bar.h"

MainWindow::MainWindow(const Tree<AudioDirectory>* audio_directories) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor")
{
    SetMinSize({ 800, 600 });

    // Components
    file_browser = new FileBrowser(this, audio_directories);
    play_bar = new PlayBar(this);

    // Layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(file_browser, 1, wxEXPAND | wxALL);
    sizer->Add(play_bar, 0, wxEXPAND | wxALL);
    SetSizer(sizer);
}
