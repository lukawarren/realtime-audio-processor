#pragma once
#include <wx/wx.h>
#include <string>

/*
    Helper class to automatically disable parent window upon creation,
    then re-enable it when the window closes.
*/

class PopupWindow : public wxFrame
{
public:
    PopupWindow(wxWindow* parent, const std::string& title) :
        wxFrame(parent, wxID_ANY, title)
    {
        // Disable parent window then re-enable parent window when we close
        GetParent()->Enable(false);
        Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
            GetParent()->Enable(true);
            event.Skip();
        });

        Show();
    }
};
