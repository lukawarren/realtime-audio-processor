#include "app.h"
#include "playlist_window.h"
#include <SDL2/SDL.h>

bool App::OnInit()
{
    // Initialise SDL audio
    SDL_Init(SDL_INIT_AUDIO);

    PlaylistWindow* window = new PlaylistWindow();
    window->Show();
    return true;
}

void App::OnUnhandledException()
{
    throw;
}
