#include "ui/play_window.h"
#include "ui/song_window.h"
#include "ui/effects_window.h"
#include "effects/effects_list.h"
#include "presets.h"
#include <wx/numdlg.h>

PlayWindow::PlayWindow(wxWindow* parent, Playlist& playlist) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor"), playlist(playlist)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    // Visualiser panel
    visualiser_panel = new AudioVisualiser(this);

    // Slider - use mouse event  (as opposed to "proper" scroll event) to avoid
    // race conditions (as it's quite possible that between clicking the slider
    // and the event being fired, the progress has already been updated by the
    // audio thread)
    progress_bar = new wxSlider(this, wxID_ANY, 0, 0, 1000000);
    progress_bar->Bind(wxEVT_LEFT_DOWN, [&](wxMouseEvent& event)
    {
        const float progress = (float)event.GetPosition().x / (float)progress_bar->GetSize().x;
        progress_bar->SetValue(progress * progress_bar->GetMax());
        audio_stream->SetProgress(progress);
    });

    // Buttons
    previous_button = new wxButton(this, wxID_ANY, "Previous");
    pause_button = new wxButton(this, wxID_ANY, "Pause");
    next_button = new wxButton(this, wxID_ANY, "Next");
    previous_button->Bind(wxEVT_BUTTON, &PlayWindow::OnPrevious, this);
    pause_button->Bind(wxEVT_BUTTON, &PlayWindow::OnPause, this);
    next_button->Bind(wxEVT_BUTTON, &PlayWindow::OnNext, this);

    // Speed dropdown
    speed_dropdown = new wxChoice(this, wxID_ANY);
    speed_dropdown->Append("0.25x");
    speed_dropdown->Append("0.5x");
    speed_dropdown->Append("0.75x");
    speed_dropdown->Append("1x");
    speed_dropdown->Append("1.25x");
    speed_dropdown->Append("1.5x");
    speed_dropdown->Append("1.75x");
    speed_dropdown->Append("2.0x");
    speed_dropdown->SetSelection(3);
    speed_dropdown->Bind(wxEVT_CHOICE, &PlayWindow::OnSpeedChanged, this);

    // Button layout
    const int margin = FromDIP(10);
    auto* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(previous_button, 0, wxLEFT | wxRIGHT, margin);
    button_sizer->Add(pause_button, 0, wxLEFT | wxRIGHT, margin);
    button_sizer->Add(next_button, 0, wxLEFT | wxRIGHT, margin);
    button_sizer->AddStretchSpacer(100);
    button_sizer->Add(speed_dropdown, 1, wxLEFT | wxRIGHT, margin);

    // Overall layout
    auto* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    vertical_sizer->Add(visualiser_panel, 1, wxEXPAND);
    vertical_sizer->Add(progress_bar, 0, wxEXPAND | wxLEFT | wxRIGHT, margin);
    vertical_sizer->Add(button_sizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER, margin);
    SetSizer(vertical_sizer);
    SetMinSize({ 500, 200 });
    SetSize({800, 500});

    // Menu / tool bar
    CreateMenuBar();

    // Bind event from audio thread to be received on main thread
    Bind(wxEVT_AUDIO_STREAM_UPDATED, [&](AudioStreamUpdatedEvent& event) {
        OnAudioStreamUpdated(event.progress, event.buffer, event.length);
    });

    // Create audio file and stream
    StartPlayback();
}

void PlayWindow::CreateMenuBar()
{
    auto* menu = new wxMenuBar();
    CreateMenu(menu, "File",            CreateFileMenu());
    CreateMenu(menu, "Effects",         CreateEffectsMenu());
    CreateMenu(menu, "Presets",         CreatePresetsMenu());
    CreateMenu(menu, "Playback",        CreatePlaybackMenu());
    CreateMenu(menu, "Visualisation",   CreateVisualisationMenu());
    SetMenuBar(menu);
}

std::vector<PlayWindow::MenuEntry> PlayWindow::CreateFileMenu()
{
    return
    {
        MenuEntry("&Select song\tCtrl-O", MENU_EVENT
        {
            // Create song pop-up window with callback
            new SongWindow
            (
                this,
                playlist,
                [&](size_t index) {
                    OnSongChanged(index);
                }
            );
        }),
        MenuEntry("&Shuffle playlist\tCtrl-S", MENU_EVENT {
            playlist.Shuffle();
        }),
        MenuEntry("&Quit\tCtrl-Q", MENU_EVENT {
            Close();
        })
    };
}

std::vector<PlayWindow::MenuEntry> PlayWindow::CreateEffectsMenu()
{
    // Create sub-menus (one item for each effect)
    int n_effects = 1;
    std::vector<MenuEntry> effects_menu_entries;
    for (const auto& effect : EFFECTS_LIST)
    {
        // Set shortcut as Ctrl-1, Ctrl-2, etc...
        effects_menu_entries.emplace_back(
            "&" + effect.first + "\tCtrl-" + std::to_string(n_effects),
            MENU_EVENT {
                // Callback; add event to list by calling function that creates instance
                effects.Add(effect.second());
            }
        );
        n_effects++;
    }

    return
    {
        MenuEntry("Add effect", MENU_EVENT {}, effects_menu_entries),
        MenuEntry("&Modify effects\tCtrl-M", MENU_EVENT
        {
            // Check there are indeed effects to modify!
            if (effects.Count() == 0)
                wxMessageBox("You do not have any active effects to modify");
            else
                new EffectsWindow(this, &effects);
        }),
        MenuEntry("&Decrease speed\tCtrl-,", MENU_EVENT
        {
            DecreaseSpeed();
        }),
        MenuEntry("&Increase speed\tCtrl-.", MENU_EVENT
        {
            IncreaseSpeed();
        }),
        MenuEntry("&Clear all effects and reset speed\tCtrl-C", MENU_EVENT
        {
            effects.Reset();
            ResetSpeed();
        })
    };
}

std::vector<PlayWindow::MenuEntry> PlayWindow::CreatePresetsMenu()
{
    std::vector<MenuEntry> entries;
    size_t i = 0;

    for (const auto& preset : EFFECT_PRESETS)
    {
        // Create name with corresponding shortcut of Alt-N
        const std::string name = "&" + preset.name + "\tAlt-" + std::to_string(i + 1);
        entries.emplace_back(MenuEntry
        {
            name,
            MENU_EVENT
            {
                // Clear current effects then add new ones
                effects.Reset();
                preset.functor(&effects);

                // Change speed
                ResetSpeed();
                if (preset.speed_steps != 0)
                {
                    const int steps = std::abs(preset.speed_steps);
                    const bool decreasing = preset.speed_steps < 0;

                    for (int i = 0; i < steps; ++i)
                    {
                        if (decreasing) DecreaseSpeed();
                        else IncreaseSpeed();
                    }
                }

            }
        });
        i += 1;
    }

    return entries;
}

std::vector<PlayWindow::MenuEntry> PlayWindow::CreatePlaybackMenu()
{
    return
    {
        MenuEntry("&Previous song\tCtrl-P", MENU_EVENT
        {
            wxCommandEvent event;
            OnPrevious(event);
        }),
        MenuEntry("&Next song\tCtrl-N", MENU_EVENT
        {
            wxCommandEvent event;
            OnNext(event);
        }),
        MenuEntry("&Toggle playback\tCtrl-Space", MENU_EVENT
        {
            wxCommandEvent event;
            OnPause(event);
        })
    };
}

std::vector<PlayWindow::MenuEntry> PlayWindow::CreateVisualisationMenu()
{
    const auto get_number = [](const std::string& name, long value, long min, long max)
    {
        long returned = wxGetNumberFromUser(
            "Please enter the new " + name,
            "",
            "Alert",
            value,
            min,
            max
        );
        return (returned == -1) ? value : returned;
    };

    return
    {
        MenuEntry("&Change frequency range\tCtrl-R", MENU_EVENT
        {
            // Humans can only hear from 20 Hz to 20,000 Hz :)
            long min = get_number("minimum frequency", 50, 20, 20000);
            long max = get_number("maximum frequency", 15000, 20, 20000);

            // Results are validated elsewhere
            visualiser_panel->SetFrequencyRange(min, max);
        }),
        MenuEntry("&Change bar width\tCtrl-W", MENU_EVENT
        {
            visualiser_panel->SetBarWidth(get_number("bar width", 1, 1, 100));
        }),
        MenuEntry("&Reset\tAlt-R", MENU_EVENT
        {
            visualiser_panel->ResetSettings();
        })
    };
}

wxMenu* PlayWindow::CreateMenu(
    wxMenuBar* menu_bar,
    const std::string& title,
    const std::vector<MenuEntry> items
)
{
    auto* menu = new wxMenu();

    for (const auto& entry : items)
    {
        // Create sub-menu if entry has children. There is no need to support
        // nested "sub-sub-menus" at this time.
        wxMenu* sub_menu = nullptr;
        if (entry.children.size() > 0)
        {
            sub_menu = new wxMenu();
            for (const auto& child : entry.children)
            {
                sub_menu->Append(menu_item_id, child.text);

                // Bind event using unique ID
                sub_menu->Bind(wxEVT_MENU, child.callback, menu_item_id);
                menu_item_id += 1;
            }
        }

        // Append entry to menu
        if (sub_menu != nullptr)
            menu->Append(menu_item_id, entry.text, sub_menu);
        else
            menu->Append(menu_item_id, entry.text);

        // Bind event using unique ID
        menu->Bind(wxEVT_MENU, entry.callback, menu_item_id);
        menu_item_id += 1;
    }

    menu_bar->Append(menu, title);
    return menu;
}


void PlayWindow::StartPlayback()
{
    // Delete audio resources if they already exist (i.e. when re-calling function)
    if (audio_stream.has_value()) audio_stream.reset();
    if (audio_file.has_value()) audio_file.reset();

    // Attempt to load audio file
    bool found_valid_audio_file = false;
    while (!found_valid_audio_file)
    {
        try
        {
            audio_file.emplace(playlist.Items()[current_song]);
            found_valid_audio_file = true;
        }
        catch (std::exception&)
        {
            wxMessageBox(
                "Failed to load audio file " + playlist.Items()[current_song] + "." +
                " One or more file(s) on disk referenced by the playlist has been modified since" +
                " it was opened. A new audio file will be chosen if possible.");

            // Remove current item from playlist in memory
            playlist.Items().erase(playlist.Items().begin() + current_song);

            // If there's nothing left in the playlist, close window
            if (playlist.Items().size() == 0)
                Destroy();

            // If we get this far, try and load the next audio file
            // We've just removed an element, so the index may be invalid
            current_song = std::min(current_song, playlist.Items().size() - 1);
        }
    }

    // Create corresponding audio stream
    audio_stream.emplace(&*audio_file, &effects);
    audio_stream->SetSpeed(GetSpeedValue());
    audio_stream->SetProgressChangedCallback([&](float progress, uint8_t* buffer, int length)
    {
        // Enqueue event on main thread
        wxQueueEvent(this, new AudioStreamUpdatedEvent(progress, buffer, length));
    });

    audio_stream->Play();
}

void PlayWindow::OnAudioStreamUpdated(float progress, uint8_t* buffer, int length)
{
    // Update UI
    progress_bar->SetValue(progress * progress_bar->GetMax());
    visualiser_panel->FeedAudio(buffer, length, audio_file.value());

    // If song has just finished, go to the next one
    if (progress == 1.0f)
    {
        wxCommandEvent event = {};
        OnNext(event);
    }
}

void PlayWindow::OnPrevious(wxCommandEvent& event)
{
    if (current_song == 0)
        current_song = playlist.Items().size();
    current_song--;

    StartPlayback();
}

void PlayWindow::OnPause(wxCommandEvent& event)
{
    if (audio_stream->IsPlaying())
    {
        audio_stream->Pause();
        pause_button->SetLabelText("Play");
    }
    else
    {
        audio_stream->Play();
        pause_button->SetLabelText("Pause");
    }
}

void PlayWindow::OnNext(wxCommandEvent& event)
{
    current_song++;
    if (current_song >= playlist.Items().size())
        current_song = 0;

    StartPlayback();
}

void PlayWindow::OnSpeedChanged(wxCommandEvent& event)
{
    if (audio_stream.has_value())
    {
        // Suspend playback (but only if a song's playing!) to avoid race conditions
        const bool should_suspend = audio_stream->IsPlaying();
        if (should_suspend)
            audio_stream->Pause();

        audio_stream->SetSpeed(GetSpeedValue());

        if (should_suspend)
            audio_stream->Play();
    }
}

void PlayWindow::OnSongChanged(const size_t index)
{
    current_song = index;
    StartPlayback();
}

float PlayWindow::GetSpeedValue() const
{
    const float speed = 0.25f + speed_dropdown->GetSelection() * 0.25f;
    return speed;
}

void PlayWindow::ResetSpeed()
{
    // Select middle selection (1x speed)
    speed_dropdown->SetSelection(
        ((int)speed_dropdown->GetCount() - 1) / 2
    );

    wxCommandEvent dummy;
    OnSpeedChanged(dummy);
}

void PlayWindow::DecreaseSpeed()
{
    wxCommandEvent dummy;
    speed_dropdown->SetSelection(
        std::max(
            speed_dropdown->GetSelection() - 1,
            0
        )
    );
    OnSpeedChanged(dummy);
}

void PlayWindow::IncreaseSpeed()
{
    wxCommandEvent dummy;
    speed_dropdown->SetSelection(
        std::min(
            speed_dropdown->GetSelection() + 1,
            (int)speed_dropdown->GetCount() - 1
        )
    );
    OnSpeedChanged(dummy);
}

PlayWindow::~PlayWindow()
{
    // We must destroy the audio stream so the audio thread isn't still
    // playing whilst everything else is destroyed
    audio_stream.reset();
    audio_file.reset();
}