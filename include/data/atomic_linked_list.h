#pragma once
#include <cassert>
#include <functional>
#include <mutex>
#include <memory>

/*
    Thread-safe linked list to avoid race conditions with audio thread
*/

template<typename T>
class AtomicLinkedList;

template<typename T>
struct ListNode
{
    T* data = nullptr;
    ListNode* next = nullptr;
    ListNode* previous = nullptr;

    ~ListNode()
    {
        if (data != nullptr)
            delete data;
    }
};

template<typename T>
class AtomicLinkedList
{
private:
    ListNode<T>* head = nullptr;
    ListNode<T>* tail = nullptr;
    std::mutex* mutex;
    size_t count = 0;

public:
    AtomicLinkedList()
    {
        mutex = new std::mutex();
    }

    void Add(T* data)
    {
        mutex->lock();

        if (head == nullptr)
        {
            // No nodes created yet; make one! :)
            head = new ListNode<T>();
            head->data = data;
            tail = head;
        }
        else
        {
            // Add as to child of current tail
            ListNode<T>* node = new ListNode<T>();
            node->data = data;
            tail->next = node;
            node->previous = tail;
            tail = node;
        }

        count++;
        mutex->unlock();
    }

    ListNode<T>* GetNthNode(const size_t index)
    {
        size_t i = 0;
        ListNode<T>* node = head;
        while (i < index)
        {
            node = node->next;
            assert(node != nullptr);
            i++;
        }

        return node;
    }

    void Remove(const size_t index)
    {
        mutex->lock();

        ListNode<T>* node = GetNthNode(index);
        ListNode<T>* previous = node->previous;
        ListNode<T>* next = node->next;

        // Update node before (if there is one!)
        if (previous != nullptr)
            previous->next = next;

        // Update node after (if there is one!)
        if (next != nullptr)
            next->previous = previous;

        // Update head and tail if need be
        if (node == head) head = next;
        if (node == tail) tail = previous;

        count--;
        delete node;
        mutex->unlock();
    }

    void ForEach(const std::function<void(T*)> callback) const
    {
        mutex->lock();

        ListNode<T>* node = head;
        while (node != nullptr)
        {
            callback(node->data);
            node = node->next;
        }

        mutex->unlock();
    }

    size_t Count() const
    {
        return count;
    }

    void SwapWithPrevious(const size_t index)
    {
        mutex->lock();
        ListNode<T>* node = GetNthNode(index);
        ListNode<T>* previous = node->previous;

        if (previous != nullptr)
        {
            auto* temp = previous->data;
            previous->data = node->data;
            node->data = temp;
        }

        mutex->unlock();
    }

    void SwapWithNext(const size_t index)
    {
        mutex->lock();
        ListNode<T>* node = GetNthNode(index);
        ListNode<T>* next = node->next;

        if (next != nullptr)
        {
            auto* temp = next->data;
            next->data = node->data;
            node->data = temp;
        }

        mutex->unlock();
    }

    // Delete all nodes
    void Reset()
    {
        mutex->lock();

        ListNode<T>* node = head;
        while (node != nullptr)
        {
            ListNode<T>* previous = node;
            node = node->next;
            delete previous;
        }

        head = nullptr;
        tail = nullptr;
        count = 0;

        mutex->unlock();
    }

    ~AtomicLinkedList()
    {
        Reset();
        delete mutex;
    }
};
