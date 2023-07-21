#pragma once
#include <wx/wx.h>
#include <wx/treelist.h>
#include <functional>
#include <optional>
#include <string>
#include "files.h"
#include "tree.h"

class FileBrowser : public wxPanel
{
public:
    FileBrowser(
        wxWindow* parent,
        const std::string& path,
        const std::function<bool(const std::string&)> is_valid_file,
        const std::function<void()> on_file_clicked
    );

    std::optional<std::string> GetSelectedFile() const;

private:
    Tree<Directory>* ScanDirectory(const std::string& path) const;
    void AddDirectory(wxTreeListItem& parent, const Tree<Directory>* directory);
    void AddFiles(wxTreeListItem& parent, const Directory* files);

    wxTreeListCtrl* tree_list;
    int list_column_file_name;
    int list_column_file_size;

    // User-defined
    std::function<bool(const std::string&)> is_valid_file;
    std::function<void()> on_file_clicked;
};
