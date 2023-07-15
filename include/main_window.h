#pragma once
#include <wx/wx.h>
#include <wx/treelist.h>
#include "audio.h"
#include "tree.h"

class MainWindow : public wxFrame
{
public:
    MainWindow(const Tree<AudioDirectory>* audio_directories);

private:
    void CreateAudioList(const Tree<AudioDirectory>* audio_directories);
    void AddDirectoryToAudioList(wxTreeListItem& parent, const Tree<AudioDirectory>* audio_directory);
    void AddFilesToAudioList(wxTreeListItem& parent, const AudioDirectory* files);

    void OnListEntryClicked();

    wxTreeListCtrl* audio_list;
    int list_column_file_name;
    int list_column_file_size;
};
