#pragma once
#include <wx/wx.h>
#include <wx/rearrangectrl.h>
#include "file_browser.h"

class PlaylistWindow : public wxFrame
{
public:
    PlaylistWindow();

private:
    // UI elements
    FileBrowser* file_browser;
    wxRearrangeCtrl* file_list;

    void OnFileAdded();
    void OnFileRemoved();
    void OnContinue() const;
    std::vector<std::string> GetItems() const;
};
