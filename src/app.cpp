#include "app.h"
#include "ui/start_window.h"
#include "ui/play_window.h"
#include <wx/stdpaths.h>
#include <SDL2/SDL.h>
#include <filesystem>
#include <iostream>

bool App::OnInit()
{
    // Initialise SDL audio and filesystem
    SDL_Init(SDL_INIT_AUDIO);
    CreateDataDirectories();

    // Get command-line arguments
    const wxCmdLineArgsArray& arguments = argv;
    const int n_arguments = argc - 1;

    if (n_arguments == 0)
    {
        // No arguments; launch playlist selection UI
        StartWindow* start_window = new StartWindow();
        start_window->Show();
    }
    else if (n_arguments == 1)
    {
        // Attempt to skip straight to loading playlist file
        std::optional<Playlist> playlist = Playlist::FromFile(arguments[1].ToStdString());

        if (playlist.has_value())
        {
            PlayWindow* window = new PlayWindow(nullptr, *playlist);
            window->Show();
        }
        else
        {
            std::cout << "Failed to load playlist as is either empty or contains non-existent audio" << std::endl;
            Exit();
        }
    }
    else
    {
        // Unknown arguments
        std::cout << "Usage: ./RAP [/path/to/playlist]" << std::endl;
        Exit();
    }

    return true;
}

void App::OnUnhandledException()
{
    // Alert user then quit
    try { throw; }
    catch (std::exception& e)
    {
        wxMessageBox(wxString(e.what()));
    }

    throw;
}

std::string App::GetAppDataPath()
{
    const wxString documents_path = wxStandardPaths::Get().GetDocumentsDir();
    const wxString app_data_path =
        documents_path +
        (std::filesystem::path::value_type)std::filesystem::path::preferred_separator +
        wxString("RAP");
    return app_data_path.ToStdString();
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