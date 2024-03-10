#include "ui/playlist_window.h"
#include "io/playlist.h"
#include "app.h"

PlaylistWindow::PlaylistWindow(const std::string& root_folder, wxWindow* parent) :
    PopupWindow(parent, "Create a playlist")
{
    SetSize({1024, 768});
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    // Create file browser and pass callback
    const auto is_valid_file = [](const std::string& filename)
    {
        return filename.size() >= 3 &&
               filename[filename.size()-3] == 'w' &&
               filename[filename.size()-2] == 'a' &&
               filename[filename.size()-1] == 'v';
    };
    const auto on_file_double_clicked = [&]() {
        auto dummy = wxCommandEvent();
        OnFileAdded(dummy);
    };
    file_browser = new FileBrowser(
        this,
        root_folder,
        is_valid_file,
        on_file_double_clicked
    );

    // Right-hand list of selected files
    file_list = new wxListBox(this, wxID_ANY);

    // Buttons
    wxButton* add_button    = new wxButton(this, wxID_ANY, "Add");
    wxButton* remove_button = new wxButton(this, wxID_ANY, "Remove");
    wxButton* next_button   = new wxButton(this, wxID_ANY, "Continue");

    // Events
    add_button->Bind(wxEVT_BUTTON, &PlaylistWindow::OnFileAdded, this);
    remove_button->Bind(wxEVT_BUTTON, &PlaylistWindow::OnFileRemoved, this);
    next_button->Bind(wxEVT_BUTTON, &PlaylistWindow::OnContinue, this);

    const int margin = FromDIP(10);

    // Top layout
    wxBoxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);
    top_sizer->Add(file_browser, 1, wxEXPAND | wxALL);
    top_sizer->Add(file_list, 1, wxEXPAND | wxALL);

    // Bottom layout
    wxBoxSizer* bottom_sizer = new wxBoxSizer(wxHORIZONTAL);
    bottom_sizer->Add(add_button, 0, wxLEFT | wxRIGHT, margin);
    bottom_sizer->Add(remove_button, 0, wxLEFT | wxRIGHT, margin);
    bottom_sizer->Add(next_button, 0, wxLEFT | wxRIGHT, margin);

    wxBoxSizer* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    vertical_sizer->Add(top_sizer, 1, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, margin);
    vertical_sizer->Add(bottom_sizer, 0, wxALIGN_CENTER | wxALL, margin);

    SetSizer(vertical_sizer);
    Layout();
}

void PlaylistWindow::OnFileAdded(wxCommandEvent& event)
{
    // Verify a file is selected (as opposed to directory, etc.)
    std::optional<std::string> filename = file_browser->GetSelectedFile();
    if (filename.has_value() && !ContainsItem(*filename))
    {
        wxString string = wxString::FromUTF8(*filename);
        file_list->InsertItems(1, &string, file_list->GetCount());
    }
}

void PlaylistWindow::OnFileRemoved(wxCommandEvent& event)
{
    long selection_index = file_list->GetSelection();
    if (selection_index != wxNOT_FOUND)
        file_list->Delete(selection_index);
}

void PlaylistWindow::OnContinue(wxCommandEvent& event)
{
    // Check playlist will be valid
    if (file_list->GetCount() < 1)
    {
        wxMessageBox("Playlists must consist of at least one file");
        return;
    }

    wxFileDialog dialog = wxFileDialog(
        this,                               // Parent
        "Save File",                        // Title
        App::GetAppPlaylistPath(),          // Default directory
        "playlist.txt",                     // Default filename
        "Text files (*.txt)|*.txt",         // Filter / wildcard
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT   // Flags
    );

    // Ignore cancels
    if (dialog.ShowModal() == wxID_CANCEL)
        return;

    // Save
    try
    {
        Playlist playlist(GetItems());
        playlist.SaveToFile(dialog.GetPath().ToStdString());
    }
    catch (std::exception&)
    {
        wxMessageBox("Failed to save playlist");
    }

    Close();
}

std::vector<std::string> PlaylistWindow::GetItems() const
{
    // Convert from vec<wxString> to vec<std::string>
    wxArrayString strings = file_list->GetStrings();
    std::vector<std::string> std_strings = {};
    std_strings.reserve(strings.size());

    for (const auto& string : strings)
        std_strings.emplace_back(string.ToStdString());

    return std_strings;
}

bool PlaylistWindow::ContainsItem(const std::string& filename) const
{
    std::vector<std::string> items = GetItems();
    return std::find(items.begin(), items.end(), filename) != items.end();
}