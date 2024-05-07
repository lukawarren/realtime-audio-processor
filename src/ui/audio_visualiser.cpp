#include "ui/audio_visualiser.h"
#include <wx/dcbuffer.h>

AudioVisualiser::AudioVisualiser(wxWindow* parent) :
    wxPanel(parent, wxID_ANY)
{
    ResetSettings();

    // Set background style to be compatible with requirements for double
    // buffering then bind custom paint event
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &AudioVisualiser::OnPaint, this);
}

void AudioVisualiser::FeedAudio(
    const uint8_t* buffer,
    const int length,
    const AudioFile& file
)
{
    // Convert from 16-bit buffer to vector of floats
    std::vector<float> audio;
    for (int i = 0; i < length / 2; ++i)
        audio.emplace_back(
            float(*((int16_t*)buffer + i)) / (float)file.MaxSampleValue()
        );

    // Perform FFT
    FastFourierTransform::GroupingSettings settings = {
        .n_buckets = GetSize().x / bar_width,
        .frequency = file.GetFrequency(),
        .minimum_audible_frequency = min_frequency,
        .maximum_audible_frequency = max_frequency
    };
    FastFourierTransform fft(audio, { settings });

    // Save old results for averaging and add new one
    for (size_t i = 0; i < fft_results.size() - 1; ++i)
        fft_results[i] = fft_results[i+1];
    fft_results[fft_results.size()-1] = fft.grouped_frequencies;

    // Redraw (will trigger paint event)
    Refresh();
}

void AudioVisualiser::SetBarWidth(const int width)
{
    bar_width = width;
}

void AudioVisualiser::SetFrequencyRange(const int min, const int max)
{
    if (min < max)
    {
        min_frequency = min;
        max_frequency = max;
    }
}

void AudioVisualiser::ResetSettings()
{
    bar_width = 1;
    min_frequency = 50;
    max_frequency = 15000;
}

void AudioVisualiser::OnPaint(const wxPaintEvent& event)
{
    const wxCoord width = GetSize().x;
    const wxCoord height = GetSize().y;

    wxAutoBufferedPaintDC context(this);

    // Background
    const wxColour background_colour(250, 250, 250, 255); // for libadwaita on gtk4 :)
    const wxBrush background_brush(background_colour, wxBRUSHSTYLE_SOLID);
    context.SetBrush(background_brush);
    context.SetPen({ background_colour, 0 });
    context.DrawRectangle(0, 0, width, height);

    // Avoid drawing before FFT data has been set by audio thread
    std::vector<FastFourierTransform::FrequencyRange>& most_recent_results = fft_results[fft_results.size()-1];
    if ((int)most_recent_results.size() < width / bar_width)
        return;

    context.SetBrush(*wxWHITE_BRUSH);
    context.SetPen({ wxColour(255, 255, 255, 255), 0 });
    context.SetBackground(*wxWHITE_BRUSH);

    // Average FFT results over multiple "frames"
    std::vector<float> magnitudes;
    magnitudes.reserve(most_recent_results.size());
    for (size_t i = 0; i < most_recent_results.size(); ++i)
    {
        float sum = 0.0f;
        for (size_t frame =  0; frame < fft_results.size(); ++frame)
        {
            if (i < fft_results[frame].size())
                sum += fft_results[frame][i].magnitude;
        }

        magnitudes.emplace_back(sum / (float)fft_results.size());
    }

    // To work out scaling, find maximum value
    float max_magnitude = 0.0f;
    for (const auto& m : magnitudes)
        if (m > max_magnitude)
            max_magnitude = m;

    const float scale = (float)height / max_magnitude;

    // Draw bars
    for (wxCoord i = 0; i < width / bar_width; ++i)
    {
        // Work out colour
        float hue = 1.0f;
        float sat = 0.0f;
        float val = (float)i / (float)(width / bar_width);
        wxColour colour = ConvertHSVToRGB(hue, sat, val);

        context.SetBrush(colour);
        context.SetPen({ colour, 0 });

        const int bar_height = magnitudes[i] * scale;
        context.DrawRectangle(
            i * bar_width,              // X
            height - bar_height - 1,    // Y
            bar_width,                  // Width
            bar_height                  // Height
        );
    }
}

/*
    Convert HSV (hue, saturation and value) to RGB (red, green, blue).
    Hue = the "base colour"
    Saturation = the "colour intensity",
    Value = the "brightness"
*/
wxColour AudioVisualiser::ConvertHSVToRGB(
    const float hue,
    const float saturation,
    const float value
)
{
    // Calculate the indices and factors needed for conversion
    const int hue_segment = int(hue * 6);
    const float fraction = hue * 6 - hue_segment;
    const float p = value * (1 - saturation);
    const float q = value * (1 - fraction * saturation);
    const float t = value * (1 - (1 - fraction) * saturation);

    // Convert HSV to RGB based on the indices
    const auto v = value;
    switch (hue_segment % 6)
    {
        case 0: return { uint8_t(v * 255), uint8_t(t * 255), uint8_t(p * 255)}; // Red to Magenta
        case 1: return { uint8_t(q * 255), uint8_t(v * 255), uint8_t(p * 255)}; // Magenta to Blue
        case 2: return { uint8_t(p * 255), uint8_t(v * 255), uint8_t(t * 255)}; // Blue to Cyan
        case 3: return { uint8_t(p * 255), uint8_t(q * 255), uint8_t(v * 255)}; // Cyan to Green
        case 4: return { uint8_t(t * 255), uint8_t(p * 255), uint8_t(v * 255)}; // Green to Yellow
        case 5: return { uint8_t(v * 255), uint8_t(p * 255), uint8_t(q * 255)}; // Yellow to Red
    }

    return {};
}

