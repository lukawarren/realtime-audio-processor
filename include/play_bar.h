#pragma once
#include <wx/wx.h>
#include <functional>

class PlayBar : public wxPanel
{
public:
    PlayBar(
        wxWindow* parent,
        const std::function<void(float)> on_progress_changed,
        std::function<void()> on_paused_or_played
    );
    void SetPlaybackProgress(const float progress);

private:
    wxButton* pause_button;
    wxSlider* progress_bar;

    // User callbacks
    std::function<void(float)> on_progress_changed;
    std::function<void()> on_paused_or_played;
};
