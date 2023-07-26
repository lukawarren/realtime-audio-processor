#pragma once
#include <cassert>
#include <functional>

template<typename T>
class ListNode;

template<typename T>
using LinkedList = ListNode<T>;

template<typename T>
class ListNode
{
protected:
    T* data = nullptr;
    ListNode* next = nullptr;
    ListNode* previous = nullptr;

public:
    ListNode() {}

    ListNode(T* data, ListNode* previous = nullptr)
    {
        this->data = data;
        this->previous = previous;
    }

    ListNode* GetLastNode()
    {
        // March through children
        ListNode* node = this;
        while (node->next != nullptr)
            node = node->next;

        return node;
    }

    void Add(T* data)
    {
        if (this->data == nullptr)
        {
            // This node is empty - use ourselves
            this->data = data;
        }
        else
        {
            // Make new node
            ListNode* last = GetLastNode();
            last->next = new ListNode<T>(data, last);
        }
    }

    void Remove()
    {
        if (previous != nullptr && next == nullptr)
        {
            // One element before us, but otherwise the last in the list
            previous->next = nullptr;
        }
        else if (previous == nullptr && next == nullptr)
        {
            // Only element in the list - no need to do anything
        }
        else
        {
            // Element both before and after us - join them together
            previous->next = next;
            next->previous = previous;
            previous = nullptr;
            next = nullptr;
        }

        delete this;
    }

    T* GetData() { return data; }

    void ForEach(const std::function<void(T* entry)> callback)
    {
        ListNode* node = this;
        while (node != nullptr)
        {
            callback(node->data);
            node = node->next;
        }
    }

    ~ListNode()
    {
        // Recursively delete children
        if (next != nullptr)
            delete next;

        // Delete ourselves
        delete data;
    }
};
