#pragma once
#include <cassert>
#include <functional>
#include <mutex>
#include <memory>

/*
    Thread-safe linked list to avoid race conditions with audio thread
*/

template<typename T>
class ListNode;

template<typename T>
using AtomicLinkedList = ListNode<T>;

template<typename T>
class ListNode
{
protected:
    T* data = nullptr;
    ListNode* next = nullptr;
    ListNode* previous = nullptr;
    std::shared_ptr<std::mutex> root_mutex;

public:
    ListNode()
    {
        // Create root mutex if need be
        if (root_mutex.use_count() == 0)
            root_mutex = std::make_shared<std::mutex>();
    }

    ListNode(T* data, ListNode* previous)
    {
        this->data = data;
        this->previous = previous;
        this->root_mutex = previous->root_mutex;
    }

    ListNode* GetLastNode()
    {
        root_mutex->lock();

        // March through children
        ListNode* node = this;
        while (node->next != nullptr)
            node = node->next;

        root_mutex->unlock();
        return node;
    }

    void Add(T* data)
    {
        root_mutex->lock();

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

        root_mutex->unlock();
    }

    void Remove()
    {
        root_mutex->lock();

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

        root_mutex->unlock();
        delete this;
    }

    T* GetData() { return data; }

    void ForEach(const std::function<void(const T* entry)> callback) const
    {
        root_mutex->lock();

        const ListNode* node = this;
        while (node != nullptr && node->data != nullptr)
        {
            callback(node->data);
            node = node->next;
        }

        root_mutex->unlock();
    }

    ~ListNode()
    {
        // Recursively delete children
        if (next != nullptr)
            delete next;

        // Delete ourselves
        if (data != nullptr)
            delete data;
    }
};
