#pragma once
#include <wx/wx.h>
#include <wx/treelist.h>
#include "audio.h"
#include "tree.h"

class FileBrowser : public wxPanel
{
public:
    FileBrowser(wxWindow* parent, const Tree<AudioDirectory>* audio_directories);

private:
    void AddDirectory(wxTreeListItem& parent, const Tree<AudioDirectory>* audio_directory);
    void AddFiles(wxTreeListItem& parent, const AudioDirectory* files);
    void OnListEntryClicked();

    wxTreeListCtrl* tree_list;
    int list_column_file_name;
    int list_column_file_size;
};
