#include "playlist_window.h"
#include "playlist.h"

PlaylistWindow::PlaylistWindow() :
    wxFrame(nullptr, wxID_ANY, "Create a playlist")
{
    SetMinSize({ 1024, 768 });
    #ifdef WIN32
    SetBackgroundColour(*wxWHITE);
    #endif

    // Create file browser and pass callback
    const auto is_valid_file = [](const std::string& filename)
    {
        return filename.size() >= 3 &&
               filename[filename.size()-3] == 'm' &&
               filename[filename.size()-2] == 'p' &&
               filename[filename.size()-1] == '3';
    };
    const auto on_file_double_clicked = [&]() {
        OnFileAdded();
    };
    file_browser = new FileBrowser(
        this,
        "/home/luka/Music",
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
    add_button->Bind(wxEVT_BUTTON, [&](wxCommandEvent& _) { OnFileAdded(); });
    remove_button->Bind(wxEVT_BUTTON, [&](wxCommandEvent& _) { OnFileRemoved(); });
    next_button->Bind(wxEVT_BUTTON, [&](wxCommandEvent& _) { OnContinue(); });

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
}

void PlaylistWindow::OnFileAdded()
{
    // Verify a file is selected (as opposed to directory, etc.)
    std::optional<std::string> filename = file_browser->GetSelectedFile();
    if (filename.has_value() && !ContainsItem(*filename))
    {
        wxString string = wxString::FromUTF8(*filename);
        file_list->InsertItems(1, &string, file_list->GetCount());
    }
}

void PlaylistWindow::OnFileRemoved()
{
    long selection_index = file_list->GetSelection();
    if (selection_index != wxNOT_FOUND)
        file_list->Delete(selection_index);
}

void PlaylistWindow::OnContinue()
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
        "",                                 // Default directory
        "playlist.txt",                     // Default filename
        "Text files (*.txt)|*.txt",         // Filter / wildcard
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT   // Flags
    );

    // Ignore cancels
    if (dialog.ShowModal() == wxID_CANCEL)
        return;

    // Save
    Playlist playlist(GetItems());
    playlist.SaveToFile(dialog.GetPath().ToStdString());
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