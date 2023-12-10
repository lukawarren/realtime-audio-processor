#include "app.h"

// If on Windows
#ifdef __WXMSW__
    extern "C" int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wxCmdLineArgType lpCmdLine, int nCmdShow)
    {
        wxApp::SetInstance(new App());
        return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }

// POSIX (Unix, etc.)
#else
    int main(int argc, char* argv[])
    {
        wxApp::SetInstance(new App());
        return wxEntry(argc, argv);
    }
#endif
