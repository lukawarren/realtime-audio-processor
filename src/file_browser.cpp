#include "file_browser.h"
#include <filesystem>
#include <string>
#include <format>

FileBrowser::FileBrowser(wxWindow* parent, const Tree<AudioDirectory>* audio_directories) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(100, 100))
{
    // Create columns
    tree_list = new wxTreeListCtrl(this, wxID_ANY);
    list_column_file_name = tree_list->AppendColumn("Path");
    list_column_file_size = tree_list->AppendColumn("Size");

    // Recursively add directories
    wxTreeListItem root = tree_list->GetRootItem();
    AddDirectory(root, audio_directories);

    tree_list->Bind(wxEVT_TREELIST_ITEM_ACTIVATED, [&](wxCommandEvent& event)
    {
        OnListEntryClicked();
    });

    // Layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(tree_list, 1, wxEXPAND | wxALL);
    SetSizerAndFit(sizer);
}

void FileBrowser::AddDirectory(wxTreeListItem& parent, const Tree<AudioDirectory>* audio_directory)
{
    // Append directory
    AudioDirectory* directory_data = audio_directory->data;
    wxTreeListItem child = tree_list->AppendItem(parent, directory_data->path);

    // Add files, using the current child as the parent
    AddFiles(child, directory_data);

    // Add children (sub-directories) recursively in the same manner :)
    for (const Tree<AudioDirectory>* sub_directory : audio_directory->children)
        AddDirectory(child, sub_directory);
}

void FileBrowser::AddFiles(wxTreeListItem& parent, const AudioDirectory* files)
{
    for (const AudioFile& file : files->files)
    {
        // Format file size to 2 decimal places
        const float size_in_mb = (float)file.size / 1000000.0f;
        const std::string file_size = std::format("{:.2f}", size_in_mb) + " MB";

        // Add to UI
        wxTreeListItem item = tree_list->AppendItem(parent, file.name, -1, -1);
        tree_list->SetItemText(item, list_column_file_name, file.name);
        tree_list->SetItemText(item, list_column_file_size, file_size);
    }
}

void FileBrowser::OnListEntryClicked()
{
    const wxTreeListItem selected_item = tree_list->GetSelection();
    const wxTreeListItem item_parent = tree_list->GetItemParent(selected_item);
    const wxString& selected_text = tree_list->GetItemText(selected_item);
    const wxString& parent_text = tree_list->GetItemText(item_parent);

    // Check selected item was an audio file and not a directory by checking it ends in .mp3
    if (selected_text.size() < 3 ||
        selected_text[selected_text.size()-3] != 'm' ||
        selected_text[selected_text.size()-2] != 'p' ||
        selected_text[selected_text.size()-1] != '3')
    {
        // If it was a directory, simply expand it
        tree_list->Expand(selected_item);
        return;
    }

    // Get full path of file by adding the directory (i.e. the parent text)
    const wxString& full_path = parent_text
        + std::filesystem::path::preferred_separator
        + selected_text;

    wxLogDebug("Selected %s", full_path);
}
