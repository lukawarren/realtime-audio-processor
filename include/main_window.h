#pragma once
#include <wx/wx.h>
#include "file_browser.h"
#include "play_bar.h"
#include "audio_stream.h"

class MainWindow : public wxFrame
{
public:
    MainWindow(const Tree<AudioDirectory>* audio_directories);
    ~MainWindow();

private:
    // UI elements
    FileBrowser* file_browser;
    PlayBar* play_bar = nullptr;

    // Currently selected audio file
    uint8_t* audio_buffer = nullptr;
    uint32_t audio_length;
    AudioStream audio_stream = {};

    void OnAudioFileSelected(const std::string& path);
};
