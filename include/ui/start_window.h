#pragma once
#include <wx/wx.h>

class StartWindow : public wxFrame
{
public:
    StartWindow();

private:
    void OnCreateButton(wxCommandEvent& event);
    void OnLoadButton(wxCommandEvent& event);
};
