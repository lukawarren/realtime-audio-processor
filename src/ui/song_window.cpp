#include "ui/song_window.h"

SongWindow::SongWindow(
    wxWindow* parent,
    Playlist& playlist,
    std::function<void(size_t)> on_song_selected
) :
    PopupWindow(parent, "Select a song"),
    on_song_selected(on_song_selected)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    // Add items to list
    list = new wxListBox(this, wxID_ANY);
    for (const auto& song : playlist.Items())
        list->Append(wxString(song));

    // Bind event
    list->Bind(wxEVT_LISTBOX, [&](wxCommandEvent& e)
    {
        int index = this->list->GetSelection();
        if (index > 0)
        {
            this->on_song_selected(index);
            Close();
        }
    });

    // Layout
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(list, 0, wxEXPAND | wxALL, FromDIP(10));
    SetSizerAndFit(sizer);
}
