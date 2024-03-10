#pragma once
#include <string>
#include <wx/wx.h>

class App : public wxApp
{
public:
    bool OnInit() override;
    void OnUnhandledException() override;

    static std::string GetAppPlaylistPath();
    static std::string GetAppAudioPath();

private:
    void CreateDataDirectories();
    static std::string EscapeRegex(const std::string& input);
};