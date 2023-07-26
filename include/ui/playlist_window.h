#pragma once
#include <wx/wx.h>
#include "ui/file_browser.h"

class PlaylistWindow : public wxFrame
{
public:
    PlaylistWindow(wxWindow* parent);

private:
    // UI elements
    FileBrowser* file_browser;
    wxListBox* file_list;

    void OnFileAdded(wxCommandEvent& event);
    void OnFileRemoved(wxCommandEvent& event);
    void OnContinue(wxCommandEvent& event);

    std::vector<std::string> GetItems() const;
    bool ContainsItem(const std::string& filename) const;
};
