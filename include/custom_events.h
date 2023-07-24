#pragma once
#include <wx/event.h>
#include <cstdint>

class AudioStreamUpdatedEvent;
wxDECLARE_EVENT(wxEVT_AUDIO_STREAM_UPDATED, AudioStreamUpdatedEvent);

class AudioStreamUpdatedEvent : public wxEvent
{
public:
    AudioStreamUpdatedEvent(float progress, uint8_t* buffer, int length) :
        wxEvent(wxID_ANY, wxEVT_AUDIO_STREAM_UPDATED)
    {
        this->progress = progress;
        this->buffer = buffer;
        this->length = length;
    }

    AudioStreamUpdatedEvent(const AudioStreamUpdatedEvent& event) :
        wxEvent(event)
    {
        progress = event.progress;
        buffer = event.buffer;
        length = event.length;
    }

    virtual wxEvent* Clone() const override
    {
        return new AudioStreamUpdatedEvent(*this);
    }

    float progress;
    uint8_t* buffer;
    int length;
};

