#include "app.h"
#include "ui/start_window.h"
#include "ui/play_window.h"
#include <wx/stdpaths.h>
#include <SDL2/SDL.h>
#include <filesystem>
#include <iostream>
#include <regex>

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
            std::cout << "Failed to load playlist" << std::endl;
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

std::string App::GetAppPlaylistPath()
{
    using namespace std::filesystem;

    const wxString documents_path = wxStandardPaths::Get().GetDocumentsDir();
    const wxString app_data_path =
        documents_path +
        (path::value_type)path::preferred_separator +
        wxString("RAP");
    return app_data_path.ToStdString();
}

std::string App::GetAppAudioPath()
{
    // wxWidgets doesn't define a default "Music" folder, so let's go to the
    // RAP folder, then go up two folders, then into the Music folder.
    // e.g. /Users/JohnSmith/Documents/RAP/ --> /Users/JohnSmith/Music
    using namespace std::filesystem;

    const std::string rap_folder = "Documents" + std::string(1, path::preferred_separator) + "RAP";
    const std::string escaped_rap_folder = EscapeRegex(rap_folder);
    const std::regex input_regex(escaped_rap_folder);

    return std::regex_replace(
        GetAppPlaylistPath(),
        input_regex,
        "Music"
    );
}

std::string App::EscapeRegex(const std::string& input)
{
    std::string output;
    for (char c : input)
    {
        // Add a "\" before special characters
        if (std::regex_match(std::string(1, c), std::regex("[\\^$|?*+()\\[\\]{}\\\\.]")))
            output += "\\";

        output += c;
    }
    return output;
}

void App::CreateDataDirectories()
{
    // Create folder if need be
    std::string path = GetAppPlaylistPath();
    using namespace std::filesystem;
    if (!is_directory(path) || !exists(path))
    {
        wxLogDebug("Creating data directory at %s", path);
        create_directory(path);
    }
}