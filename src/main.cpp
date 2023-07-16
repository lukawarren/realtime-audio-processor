#include "app.h"

#ifdef __WXMSW__
    extern "C" int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wxCmdLineArgType lpCmdLine, int nCmdShow)
    {
        wxApp::SetInstance(new App());
        return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }
#else
    int main(int argc, char* argv[])
    {
        wxApp::SetInstance(new App());
        return wxEntry(argc, argv);
    }
#endif
