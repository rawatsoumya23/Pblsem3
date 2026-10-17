#include <cassert>
#include <iostream>
#include <string>
#include "ds/LinkedList.h"

// Helper: turn a list of ints into text like "1 2 3 " so the order is easy to check.
std::string toText(const LinkedList<int>& list) {
    std::string text;
    list.forEach([&text](int x) { text += std::to_string(x) + " "; });
    return text;
}

// A small record, like an entry in the incident history.
struct Record {
    int id;
    std::string type;
};

int main() {
    // Test 1: a new list is empty
    LinkedList<int> list;
    assert(list.empty());
    assert(list.size() == 0);
    assert(toText(list) == "");

    // Test 2: pushBack keeps items in order
    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);
    assert(toText(list) == "1 2 3 ");

    // Test 3: pushFront adds at the start
    list.pushFront(0);
    assert(toText(list) == "0 1 2 3 ");
    assert(list.size() == 4);

    // Test 4: findIf finds an existing item, and the pointer changes the real item
    int* found = list.findIf([](int x) { return x == 2; });
    assert(found != nullptr && *found == 2);
    *found = 20;
    assert(toText(list) == "0 1 20 3 ");

    // Test 5: findIf returns nullptr when nothing matches
    assert(list.findIf([](int x) { return x == 99; }) == nullptr);

    // Test 6: remove from the middle, the head, and the tail
    assert(list.removeIf([](int x) { return x == 20; }));   // middle
    assert(toText(list) == "0 1 3 ");
    assert(list.removeIf([](int x) { return x == 0; }));    // head
    assert(toText(list) == "1 3 ");
    assert(list.removeIf([](int x) { return x == 3; }));    // tail
    assert(toText(list) == "1 ");
    list.pushBack(4);                                        // tail_ must be correct after removing the tail
    assert(toText(list) == "1 4 ");

    // Test 7: removing something that does not exist returns false
    assert(!list.removeIf([](int x) { return x == 99; }));
    assert(list.size() == 2);

    // Test 8: remove the ONLY item, then use the list again
    LinkedList<int> single;
    single.pushBack(5);
    assert(single.removeIf([](int x) { return x == 5; }));
    assert(single.empty());
    single.pushBack(6);       // fails if head_/tail_ were not reset
    single.pushFront(7);
    assert(toText(single) == "7 6 ");

    // Test 9: works with records, searched by ID (like incident history)
    LinkedList<Record> history;
    history.pushBack({1, "Fire"});
    history.pushBack({2, "Collapse"});
    history.pushBack({3, "Medical"});
    Record* r = history.findIf([](const Record& rec) { return rec.id == 2; });
    assert(r != nullptr && r->type == "Collapse");

    // Test 10: big test - 100000 items added, then removed from the front
    LinkedList<int> big;
    for (int i = 0; i < 100000; ++i) big.pushBack(i);
    assert(big.size() == 100000);
    for (int i = 0; i < 100000; ++i) {
        int expected = i;
        assert(big.removeIf([expected](int x) { return x == expected; }));
    }
    assert(big.empty());

    std::cout << "All LinkedList tests passed\n";
    return 0;
}
