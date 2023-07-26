#pragma once
#include <wx/wx.h>
#include <cstdint>
#include <vector>
#include <array>
#include "effects/fft.h"
#include "io/audio_file.h"

class AudioVisualiser : public wxPanel
{
public:
    AudioVisualiser(wxWindow* parent);

    // Perform FFT and trigger re-draw of UI
    void FeedAudio(
        const uint8_t* buffer,
        const int length,
        const AudioFile& file
    );

private:
    void OnPaint(const wxPaintEvent& event);

    wxColour ConvertHSVToRGB(
        const float h,
        const float s,
        const float v
    );

    // Results of FFTs over multiple "frames"
    std::array<std::vector<FastFourierTransform::FrequencyRange>, 8> fft_results = {};
};