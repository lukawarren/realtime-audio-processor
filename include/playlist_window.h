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
    wxRearrangeList* file_list;
};
