#pragma once
#include <wx/wx.h>

class PlayBar : public wxPanel
{
public:
    PlayBar(wxWindow* parent);

    void SetPlaybackProgress(const float progress);

private:
    wxGauge* progress_bar;
};
