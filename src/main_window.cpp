#include "main_window.h"
#include "tree.h"
#include <string>
#include <format>

MainWindow::MainWindow(const Tree<AudioDirectory>* audio_directories) :
    wxFrame(nullptr, wxID_ANY, "Realtime Audio Processor")
{
    CreateAudioList(audio_directories);

    // Insert rest of UI code here
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
        wxTreeListItem item = audio_list->AppendItem(parent, file.name);
        audio_list->SetItemText(item, list_column_file_name, file.name);
        audio_list->SetItemText(item, list_column_file_size, file_size);
    }
}