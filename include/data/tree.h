#pragma once
#include <vector>
#include <iostream>

template<typename T>
class Tree
{
public:
    Tree() {}
    Tree(T* data) : data(data) {}

    ~Tree()
    {
        // Recursively delete children
        for (const Tree* tree : children)
        {
            delete tree;
        }

        // Delete ourselves
        delete data;
    }

    Tree* AddChild(T* data)
    {
        Tree* child = new Tree(data);
        children.emplace_back(child);
        return child;
    }

    Tree* AddChild(Tree* tree)
    {
        children.emplace_back(tree);
        return tree;
    }

    void SetNode(T* data)
    {
        this->data = data;
    }

    T* GetNode() const
    {
        return data;
    }

    T* data = nullptr;
    std::vector<Tree*> children = {};
};