#pragma once
#include <wx/wx.h>
#include "file_browser.h"
#include "play_bar.h"

class MainWindow : public wxFrame
{
public:
    MainWindow(const Tree<AudioDirectory>* audio_directories);

private:
    FileBrowser* file_browser;
    PlayBar* play_bar;
};
