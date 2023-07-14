#pragma once
#include <string>
#include <wx/wx.h>
#include "audio.h"
#include "tree.h"

class App : public wxApp
{
public:
    bool OnInit() override;
    void OnUnhandledException() override;
    ~App();

private:
    Tree<AudioDirectory>* GetAudioDirectories(const std::string& path) const;
    Tree<AudioDirectory>* audio_directories;
};