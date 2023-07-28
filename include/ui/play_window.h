#pragma once
#include <wx/wx.h>
#include <optional>
#include "io/playlist.h"
#include "io/audio_file.h"
#include "io/audio_stream.h"
#include "ui/custom_events.h"
#include "ui/audio_visualiser.h"

class PlayWindow : public wxFrame
{
public:
    PlayWindow(wxWindow* parent, const Playlist& playlist);

private:

    // Menu bar types
    typedef std::function<void(wxCommandEvent&)> MenuCallback;
    struct MenuEntry
    {
        std::string text;
        MenuCallback callback;
        std::vector<MenuEntry> children = {};
    };
    #define MENU_EVENT [&](wxCommandEvent& e)

    // Menu bar
    void CreateMenuBar();
    std::vector<MenuEntry> CreateMiscMenu();
    std::vector<MenuEntry> CreateEffectsMenu();
    std::vector<MenuEntry> CreatePlaybackMenu();
    std::vector<MenuEntry> CreateVisualisationMenu();
    wxMenu* CreateMenu(
        wxMenuBar* menu_bar,
        const std::string& title,
        const std::vector<MenuEntry> items
    );
    int menu_item_id = 0;

    void StartPlayback();

    // Events
    void OnAudioStreamUpdated(float progress, uint8_t* buffer, int length);
    void OnPrevious(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnNext(wxCommandEvent& event);

    AudioVisualiser* visualiser_panel;
    wxSlider* progress_bar;
    wxButton* previous_button;
    wxButton* pause_button;
    wxButton* next_button;

    // Playlist
    Playlist playlist;
    size_t current_song = 0;

    // Playback
    std::optional<AudioFile> audio_file = {};
    std::optional<AudioStream> audio_stream = {};
    AtomicLinkedList<AudioEffect> effects = {};
};
