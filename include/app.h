#pragma once
#include <string>
#include <wx/wx.h>

class App : public wxApp
{
public:
    bool OnInit() override;
    void OnUnhandledException() override;

    static std::string GetAppDataPath();

private:
    void CreateDataDirectories();
};