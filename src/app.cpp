#include "app.h"
#include "main_window.h"
#include <filesystem>

bool App::OnInit()
{
    // Scan for audio files
    audio_directories = GetAudioDirectories("/home/luka/Music");

    MainWindow* window = new MainWindow(audio_directories);
    window->Show();
    return true;
}

void App::OnUnhandledException()
{
    throw;
}

App::~App()
{
    delete audio_directories;
}

Tree<AudioDirectory>* App::GetAudioDirectories(const std::string& path) const
{
    // Create empty tree of directories, where each directory (node) has
    // as its data the files contained within it
    Tree<AudioDirectory>* files = new Tree<AudioDirectory>();

    // Set root data as root node
    AudioDirectory* root = new AudioDirectory { .path = path };
    files->SetNode(root);

    // Iterate through directories in root
    using namespace std::filesystem;
    for (const auto& entry : directory_iterator(path))
    {
        if (is_directory(entry))
        {
            // Recursively get files in sub-folder so as to add as branch of tree
            Tree<AudioDirectory>* subtree = GetAudioDirectories(entry.path().string());
            files->AddChild(subtree);
        }
        else if (is_regular_file(entry) && entry.path().extension().string() == ".mp3")
        {
            // MP3 file found; append to root data
            AudioFile file {
                .name = entry.path().filename().string(),
                .size = file_size(entry)
            };
            root->files.emplace_back(file);
        }
    }

    return files;
}
