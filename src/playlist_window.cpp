#include "playlist_window.h"

PlaylistWindow::PlaylistWindow() :
    wxFrame(nullptr, wxID_ANY, "Create a playlist")
{
    SetMinSize({ 800, 600 });

    // Create file browser and pass callback
    const auto is_valid_file = [](const std::string& filename)
    {
        return filename.size() >= 3 &&
               filename[filename.size()-3] == 'm' &&
               filename[filename.size()-2] == 'p' &&
               filename[filename.size()-1] == '3';
    };
    file_browser = new FileBrowser(
        this,
        "/home/luka/src/luka/coursework/playlists",
        is_valid_file,
        [&](const std::string& path) {}
    );

    // Right-hand list of selected files
    file_list = new wxRearrangeList(
        this,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxArrayInt(0),
        wxArrayString(0)
    );

    // Buttons
    wxButton* add_button    = new wxButton(this, wxID_ANY, "Add");
    wxButton* remove_button = new wxButton(this, wxID_ANY, "Remove");
    wxButton* up_button     = new wxButton(this, wxID_ANY, "Move Up");
    wxButton* down_button   = new wxButton(this, wxID_ANY, "Move Down");

    const int margin = FromDIP(10);

    // Top layout
    wxBoxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);
    top_sizer->Add(file_browser, 2, wxEXPAND | wxALL);
    top_sizer->Add(file_list, 1, wxEXPAND | wxALL);

    // Bottom layout
    wxBoxSizer* bottom_sizer = new wxBoxSizer(wxHORIZONTAL);
    bottom_sizer->Add(add_button,       0, wxALL, margin);
    bottom_sizer->Add(remove_button,    0, wxALL, margin);
    bottom_sizer->Add(up_button,        0, wxALL, margin);
    bottom_sizer->Add(down_button,      0, wxALL, margin);

    wxBoxSizer* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    vertical_sizer->Add(top_sizer, 1, wxEXPAND | wxALL);
    vertical_sizer->Add(bottom_sizer, 0, wxALIGN_CENTER | wxALL);
    SetSizer(vertical_sizer);
}
