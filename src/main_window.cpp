#include "main_window.h"

MainWindow::MainWindow(const Tree<AudioDirectory>* audio_directories) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor")
{
    file_browser = new FileBrowser(this, audio_directories);
}
