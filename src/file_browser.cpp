#include "file_browser.h"
#include <filesystem>
#include <string>
#include <format>

FileBrowser::FileBrowser(
    wxWindow* parent,
    const std::string& path,
    std::function<bool(const std::string&)> is_valid_file,
    std::function<void()> on_file_clicked
) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(100, 100)),
    is_valid_file(is_valid_file),
    on_file_clicked(on_file_clicked)
{
#ifdef WIN32
    SetBackgroundColour(*wxWHITE);
#endif

    // Create columns
    tree_list = new wxTreeListCtrl(this, wxID_ANY);
    list_column_file_name = tree_list->AppendColumn("Path");
    list_column_file_size = tree_list->AppendColumn("Size");

    // Recursively enumerate then add directories
    wxTreeListItem root = tree_list->GetRootItem();
    Tree<Directory>* directories = ScanDirectory(path);
    AddDirectory(root, directories);
    delete directories;

    // User callback event
    tree_list->Bind(wxEVT_TREELIST_ITEM_ACTIVATED, [&](wxCommandEvent& event)
    {
        // Verify file clicked
        if (GetSelectedFile().has_value())
            this->on_file_clicked();
        else
            tree_list->Expand(tree_list->GetSelection());
    });

    // Layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(tree_list, 1, wxEXPAND | wxALL);
    SetSizerAndFit(sizer);
}

Tree<Directory>* FileBrowser::ScanDirectory(const std::string& path) const
{
    // Create empty tree of directories, where each directory (node) has
    // as its data the files contained within it
    Tree<Directory>* files = new Tree<Directory>();

    // Set root data as root Directory
    Directory* root = new Directory { .path = path };
    files->SetNode(root);

    // Iterate through directories in root
    using namespace std::filesystem;
    for (const auto& entry : directory_iterator(path))
    {
        if (is_directory(entry))
        {
            // Recursively get files in sub-folder so as to add as branch of tree
            Tree<Directory>* subtree = ScanDirectory(entry.path().string());
            files->AddChild(subtree);
        }
        else if (is_regular_file(entry) && is_valid_file(entry.path().string()))
        {
            // Audio file found; append to root data
            File file {
                .name = entry.path().filename().string(),
                .size = file_size(entry)
            };
            root->files.emplace_back(file);
        }
    }

    return files;
}

void FileBrowser::AddDirectory(wxTreeListItem& parent, const Tree<Directory>* audio_directory)
{
    // Append directory
    Directory* directory_data = audio_directory->data;
    wxTreeListItem child = tree_list->AppendItem(parent, directory_data->path);

    // Add files, using the current child as the parent
    AddFiles(child, directory_data);

    // Add children (sub-directories) recursively in the same manner :)
    for (const Tree<Directory>* sub_directory : audio_directory->children)
        AddDirectory(child, sub_directory);
}

void FileBrowser::AddFiles(wxTreeListItem& parent, const Directory* files)
{
    for (const File& file : files->files)
    {
        // Format file size to 2 decimal places
        const float size_in_mb = (float)file.size / 1000000.0f;
        const std::string file_size = std::format("{:.2f}", size_in_mb) + " MB";

        // Add to UI
        wxTreeListItem item = tree_list->AppendItem(parent, file.name, -1, -1);
        tree_list->SetItemText(item, list_column_file_name, file.name);
        tree_list->SetItemText(item, list_column_file_size, file_size);
    }
}

std::optional<std::string> FileBrowser::GetSelectedFile() const
{
    const wxTreeListItem selected_item = tree_list->GetSelection();
    const wxTreeListItem item_parent = tree_list->GetItemParent(selected_item);
    const wxString& selected_text = tree_list->GetItemText(selected_item);
    const wxString& parent_text = tree_list->GetItemText(item_parent);

    // Check selected item was a file
    if (!is_valid_file(selected_text.ToStdString()))
        return std::nullopt;

    // Get full path of file by adding the directory (i.e. the parent text)
    const wxString full_path = parent_text
        + (std::filesystem::path::value_type)std::filesystem::path::preferred_separator
        + selected_text;

    return full_path.ToStdString();
}
