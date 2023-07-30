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
    std::shared_ptr<std::mutex> root_mutex = {};

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

    ListNode(const ListNode&) = delete;

    // Only need mutex if not already called from thread-safe environment
     // (i.e. if mutex not already locked)
    ListNode* GetLastNode(bool use_mutex = true)
    {
        if (use_mutex)
            root_mutex->lock();

        // March through children
        ListNode* node = this;
        while (node->next != nullptr)
            node = node->next;

        if (use_mutex)
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
            ListNode* last = GetLastNode(false);
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
            root_mutex->unlock();
            delete this;
            return;
        }

        if (previous == nullptr && next == nullptr)
        {
            // Only element in the list - wipe data but stay in memory
            data = nullptr;
            root_mutex->unlock();
            return;
        }

        // Element both before and after us - join them together
        previous->next = next;
        next->previous = previous;
        previous = nullptr;
        next = nullptr;
        root_mutex->unlock();
        delete this;
    }

    T* GetData() { return data; }

    void ForEach(const std::function<void(T* entry)> callback) const
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

    size_t Count() const
    {
        size_t count = 0;
        ForEach([&](const T* entry) {
            count++;
        });
        return count;
    }

    ListNode* GetNthItem(const size_t n)
    {
        root_mutex->lock();
        ListNode* node = this;
        size_t i = 0;

        while (node != nullptr && node->data != nullptr && i < n)
        {
            node = node->next;
            i++;
        }

        root_mutex->unlock();
        return node;
    }

    void SwapWithPrevious()
    {
        root_mutex->lock();

        if (previous != nullptr)
        {
            T* temp = data;
            data = previous->data;
            previous->data = temp;
        }

        root_mutex->unlock();
    }

    void SwapWithNext()
    {
        root_mutex->lock();

        if (next != nullptr)
        {
            T* temp = data;
            data = next->data;
            next->data = temp;
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
