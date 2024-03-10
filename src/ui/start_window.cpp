#include "ui/start_window.h"
#include "ui/playlist_window.h"
#include "ui/play_window.h"
#include <wx/stattext.h>
#include "app.h"

StartWindow::StartWindow() : wxFrame(nullptr, wxID_ANY, "Choose a playlist")
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    // Welcome text
    auto* text = new wxStaticText(
        this,
        wxID_ANY,
        "Welcome to the Realtime Audio Processor. "
        "Please pick a playlist to get started.",
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_CENTRE_HORIZONTAL | wxST_ELLIPSIZE_END
    );

    // Buttons
    auto* load_button = new wxButton(this, wxID_ANY, "Load a playlist");
    auto* create_button = new wxButton(this, wxID_ANY, "Create a playlist");
    load_button->Bind(wxEVT_BUTTON, &StartWindow::OnLoadButton, this);
    create_button->Bind(wxEVT_BUTTON, &StartWindow::OnCreateButton, this);

    // Layout
    const int margin = FromDIP(10);
    auto* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    vertical_sizer->Add(text, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, margin);
    vertical_sizer->Add(load_button, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, margin);
    vertical_sizer->Add(create_button, 0, wxEXPAND | wxALL, margin);
    SetSizerAndFit(vertical_sizer);
}

void StartWindow::OnCreateButton(wxCommandEvent& event)
{
    new PlaylistWindow(App::GetAppAudioPath(), this);
}

void StartWindow::OnLoadButton(wxCommandEvent& event)
{
    wxFileDialog dialog = wxFileDialog(
        this,                               // Parent
        "Open playlist",                    // Title
        App::GetAppPlaylistPath(),          // Default directory
        "playlist.txt",                     // Default filename
        "Text files (*.txt)|*.txt",         // Filter / wildcard
        wxFD_OPEN                           // Flags
    );

    // Ignore cancels
    if (dialog.ShowModal() == wxID_CANCEL)
        return;

    // Attempt to create playlist (may not succeed if has missing audio files)
    std::optional<Playlist> playlist = Playlist::FromFile(dialog.GetPath().ToStdString());

    if (playlist.has_value())
    {
        // Open play window
        PlayWindow* window = new PlayWindow(this, *playlist);
        window->Show();

        // Hide current window, but close it when the above window closes (so as to
        // close the app as a whole)
        Hide();
        window->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
            this->Close();
            event.Skip();
        });
    }
    else
    {
        wxMessageBox("Failed to load playlist");
    }
}