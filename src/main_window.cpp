#include "main_window.h"
#include "tree.h"
#include <string>
#include <format>
#include <filesystem>

MainWindow::MainWindow(const Tree<AudioDirectory>* audio_directories) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor")
{
    CreateAudioList(audio_directories);

    audio_list->Bind(wxEVT_TREELIST_ITEM_ACTIVATED, [&](wxCommandEvent& event)
    {
        OnListEntryClicked();
    });
}

void MainWindow::CreateAudioList(const Tree<AudioDirectory>* audio_directories)
{
    // Create columns
    audio_list = new wxTreeListCtrl(this, wxID_ANY);
    list_column_file_name = audio_list->AppendColumn("Path");
    list_column_file_size = audio_list->AppendColumn("Size");

    // Recursively add directories
    wxTreeListItem root = audio_list->GetRootItem();
    AddDirectoryToAudioList(root, audio_directories);
}

void MainWindow::AddDirectoryToAudioList(wxTreeListItem& parent, const Tree<AudioDirectory>* audio_directory)
{
    // Append directory
    AudioDirectory* directory_data = audio_directory->data;
    wxTreeListItem child = audio_list->AppendItem(parent, directory_data->path);

    // Add files, using the current child as the parent
    AddFilesToAudioList(child, directory_data);

    // Add children (sub-directories) recursively in the same manner :)
    for (const Tree<AudioDirectory>* sub_directory : audio_directory->children)
        AddDirectoryToAudioList(child, sub_directory);
}

void MainWindow::AddFilesToAudioList(wxTreeListItem& parent, const AudioDirectory* files)
{
    for (const AudioFile& file : files->files)
    {
        // Format file size to 2 decimal places
        const float size_in_mb = (float)file.size / 1000000.0f;
        const std::string file_size = std::format("{:.2f}", size_in_mb) + " MB";

        // Add to UI
        wxTreeListItem item = audio_list->AppendItem(parent, file.name, -1, -1);
        audio_list->SetItemText(item, list_column_file_name, file.name);
        audio_list->SetItemText(item, list_column_file_size, file_size);
    }
}

void MainWindow::OnListEntryClicked()
{
    const wxTreeListItem selected_item = audio_list->GetSelection();
    const wxTreeListItem item_parent = audio_list->GetItemParent(selected_item);
    const wxString& selected_text = audio_list->GetItemText(selected_item);
    const wxString& parent_text = audio_list->GetItemText(item_parent);

    // Check selected item was an audio file and not a directory by checking it ends in .mp3
    if (selected_text.size() < 3 ||
        selected_text[selected_text.size()-3] != 'm' ||
        selected_text[selected_text.size()-2] != 'p' ||
        selected_text[selected_text.size()-1] != '3')
    {
        // If it was a directory, simply expand it
        audio_list->Expand(selected_item);
        return;
    }

    // Get full path of file by adding the directory (i.e. the parent text)
    const wxString& full_path = parent_text
        + std::filesystem::path::preferred_separator
        + selected_text;

    wxLogDebug("Selected %s", full_path);
}
