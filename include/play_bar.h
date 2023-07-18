#pragma once
#include <wx/wx.h>
#include <functional>

class PlayBar : public wxPanel
{
public:
    PlayBar(wxWindow* parent, const std::function<void(float)> on_progress_changed);
    void SetPlaybackProgress(const float progress);

private:
    wxSlider* progress_bar;

    // User callback
    std::function<void(float)> on_progress_changed;
};
