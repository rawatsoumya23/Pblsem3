#pragma once
#include <cstddef>

// LinkedList<T>: a chain of nodes where each node points to the next one.
//
//   head_ -> [A] -> [B] -> [C] -> nullptr
//                           ^
//                         tail_
//
// Unlike Queue, items can be searched and removed from ANYWHERE in the list.
//
// Used in RakshaNetra for:
//   - incident history (new incidents added at the end, searched by ID)
//   - lists of workers inside each zone (for headcount and roll-call)
//
// Time complexity:
//   pushBack, pushFront          O(1)
//   findIf, removeIf, forEach    O(n)  (may need to walk the whole list)
template <typename T>
class LinkedList {
private:
    struct Node {
        T value;
        Node* next;   // next node in the chain (nullptr for the last node)
    };

    Node* head_ = nullptr;    // first node
    Node* tail_ = nullptr;    // last node (lets pushBack be O(1))
    std::size_t size_ = 0;

public:
    LinkedList() = default;

    // Destructor: frees every node so no memory is leaked.
    ~LinkedList() { clear(); }

    // Copying is switched off: two lists sharing nodes would both delete them.
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    // Add an item at the end. Same logic as Queue::enqueue.
    void pushBack(const T& value) {
        Node* node = new Node{value, nullptr};
        if (empty()) {
            head_ = node;
            tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    // Add an item at the start.
    void pushFront(const T& value) {
        Node* node = new Node{value, head_};   // new node points to the old first node
        head_ = node;
        if (tail_ == nullptr) tail_ = node;    // list was empty: it is also the last node
        ++size_;
    }

    // Return a pointer to the first item where match(item) is true,
    // or nullptr if no item matches. The pointer lets the caller
    // read or change the item that is stored inside the list.
    template <typename F>
    T* findIf(F match) {
        for (Node* cur = head_; cur != nullptr; cur = cur->next) {
            if (match(cur->value)) return &cur->value;
        }
        return nullptr;
    }

    // Remove the first item where match(item) is true.
    // Returns true if an item was removed, false if nothing matched.
    template <typename F>
    bool removeIf(F match) {
        Node* prev = nullptr;     // the node before cur (nullptr while cur is the head)
        Node* cur = head_;
        while (cur != nullptr) {
            if (match(cur->value)) {
                if (prev == nullptr) {
                    head_ = cur->next;        // removing the first node
                } else {
                    prev->next = cur->next;   // skip over cur
                }
                if (cur == tail_) {
                    tail_ = prev;             // removing the last node: prev is the new last
                }                             // (prev is nullptr if this was the only node)
                delete cur;
                --size_;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    // Call action(item) on every item, from first to last.
    template <typename F>
    void forEach(F action) const {
        for (const Node* cur = head_; cur != nullptr; cur = cur->next) {
            action(cur->value);
        }
    }

    // Delete every node and make the list empty.
    void clear() {
        while (head_ != nullptr) {
            Node* next = head_->next;   // remember the next node BEFORE deleting this one
            delete head_;
            head_ = next;
        }
        tail_ = nullptr;
        size_ = 0;
    }

    bool empty() const { return head_ == nullptr; }
    std::size_t size() const { return size_; }
};
