#pragma once
#include <wx/wx.h>
#include "file_browser.h"

class PlaylistWindow : public wxFrame
{
public:
    PlaylistWindow();

private:
    // UI elements
    FileBrowser* file_browser;
    wxListBox* file_list;

    void OnFileAdded();
    void OnFileRemoved();
    void OnContinue();

    std::vector<std::string> GetItems() const;
    bool ContainsItem(const std::string& filename) const;
};
