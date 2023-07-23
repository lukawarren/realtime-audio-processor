#include "app.h"
#include "start_window.h"
#include "play_window.h"
#include <wx/stdpaths.h>
#include <SDL2/SDL.h>
#include <filesystem>

bool App::OnInit()
{
    // Initialise SDL audio and filesystem
    SDL_Init(SDL_INIT_AUDIO);
    CreateDataDirectories();

    StartWindow* start_window = new StartWindow();
    start_window->Show();
    //(new PlayWindow(nullptr, Playlist::FromFile("/home/luka/Documents/RAP/playlist.txt")))->Show();

    return true;
}

void App::OnUnhandledException()
{
    throw;
}

std::string App::GetAppDataPath()
{
    const wxString documents_path = wxStandardPaths::Get().GetDocumentsDir();
    const std::string app_data_path =
        documents_path.ToStdString() +
        std::to_string(std::filesystem::path::preferred_separator) +
        "RAP";
    return app_data_path;
}

void App::CreateDataDirectories()
{
    // Create folder if need be
    std::string path = GetAppDataPath();
    using namespace std::filesystem;
    if (!is_directory(path) || !exists(path))
    {
        wxLogDebug("Creating data directory at %s", path);
        create_directory(path);
    }
}