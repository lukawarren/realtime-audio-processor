#include "main_window.h"
#include "file_browser.h"
#include "play_bar.h"
#include <SDL2/SDL.h>

MainWindow::MainWindow(const Tree<AudioDirectory>* audio_directories) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor")
{
    SetMinSize({ 800, 600 });

    // Create file browser and pass callback
    file_browser = new FileBrowser(this, audio_directories, [&](const std::string& path)
    {
        OnAudioFileSelected(path);
    });

    // Layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(file_browser, 1, wxEXPAND | wxALL);
    SetSizer(sizer);
}

void MainWindow::OnAudioFileSelected(const std::string& path)
{
    // Free previous audio (if any)
    if (audio_buffer != nullptr)
        SDL_FreeWAV(audio_buffer);
    if (audio_stream != nullptr)
        delete audio_stream;

    // Remove old playbar (if any)
    if (play_bar != nullptr)
        delete play_bar;

    // Add playbar
    play_bar = new PlayBar(this);
    GetSizer()->Add(play_bar, 0, wxEXPAND | wxALL);
    Layout();

    // Load audio from disk
    SDL_AudioSpec properties;
    uint32_t length;
    uint8_t* buffer;
    if (SDL_LoadWAV(path.c_str(), &properties, &buffer, &length) == NULL)
        throw std::runtime_error("Failed to load " + path);

    // Check format is supported
    // TODO: add more supported formats
    if (properties.format != AUDIO_S16LSB)
        throw std::runtime_error("Unsupported audio format");

    // Make new audio stream from audio properties and data
    CreateAudioStream(properties, buffer, length);
    audio_stream->Play();
}

void MainWindow::CreateAudioStream(SDL_AudioSpec properties, uint8_t* buffer, uint32_t length)
{
    audio_stream = new AudioStream(
        properties.freq,
        properties.format,
        properties.channels
    );

    audio_stream->SetInputData(buffer, length);

    // Update UI when audio stream is played
    audio_stream->SetProgressChangedCallback([&](float progress)
    {
        if (play_bar != nullptr)
            play_bar->SetPlaybackProgress(progress);
    });
}

MainWindow::~MainWindow()
{
    if (audio_buffer != nullptr)
        SDL_FreeWAV(audio_buffer);

    if (audio_stream != nullptr)
        delete audio_stream;
}