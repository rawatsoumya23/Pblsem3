#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "core/Enums.h"
#include "ds/BinaryHeap.h"

// Comparators: less(a, b) = "a has LOWER priority than b"
struct BiggestFirst  { bool operator()(int a, int b) const { return a < b; } };   // max-heap
struct SmallestFirst { bool operator()(int a, int b) const { return a > b; } };   // min-heap

// A simplified emergency: just what the priority queue needs.
struct Emergency {
    int id = 0;
    Severity severity = Severity::Minor;
    int seq = 0;   // report order (we use the AUTO_INCREMENT emergency_id from MySQL)
};

// Emergency priority rule:
//   1. higher severity first
//   2. same severity -> reported earlier (smaller seq) first
struct EmergencyOrder {
    bool operator()(const Emergency& a, const Emergency& b) const {
        if (a.severity != b.severity) return a.severity < b.severity;
        return a.seq > b.seq;   // a reported LATER -> a has lower priority
    }
};

int main() {
    // Test 1: empty heap
    BinaryHeap<int, BiggestFirst> h;
    assert(h.empty() && h.size() == 0);
    bool threw = false;
    try { h.pop(); } catch (const std::underflow_error&) { threw = true; }
    assert(threw);

    // Test 2: max-heap gives the biggest first, whatever the insert order
    int values[] = {5, 1, 9, 3, 7, 2, 8};
    for (int v : values) h.push(v);
    assert(h.size() == 7 && h.top() == 9);
    int expectedMax[] = {9, 8, 7, 5, 3, 2, 1};
    for (int e : expectedMax) assert(h.pop() == e);
    assert(h.empty());

    // Test 3: min-heap (used later by Dijkstra) gives the smallest first
    BinaryHeap<int, SmallestFirst> mh;
    for (int v : values) mh.push(v);
    int expectedMin[] = {1, 2, 3, 5, 7, 8, 9};
    for (int e : expectedMin) assert(mh.pop() == e);

    // Test 4: one item in, one item out (pop with a single item)
    mh.push(42);
    assert(mh.pop() == 42 && mh.empty());

    // Test 5: the RakshaNetra demo emergencies, in the order they were reported.
    // Severities are what the severity rule gives for each demo row.
    BinaryHeap<Emergency, EmergencyOrder> queue;
    queue.push({1,  Severity::Moderate, 1});    // Electrical, 2 people
    queue.push({2,  Severity::Critical, 2});    // Fire in tunnel, trapped
    queue.push({3,  Severity::Critical, 3});    // Collapse, trapped + unconscious
    queue.push({4,  Severity::Serious,  4});    // Medical, unconscious
    queue.push({5,  Severity::Minor,    5});    // Fall
    queue.push({6,  Severity::Moderate, 6});    // Vehicle, 2 people
    queue.push({7,  Severity::Moderate, 7});    // Fire in yard
    queue.push({8,  Severity::Minor,    8});    // Medical, unknown reporter
    queue.push({9,  Severity::Critical, 9});    // Collapse in tunnel, trapped
    queue.push({10, Severity::Serious,  10});   // Medical, unconscious, 2 people

    // Expected dispatch order: all Critical (in report order), then Serious, Moderate, Minor.
    int expectedOrder[] = {2, 3, 9, 4, 10, 1, 6, 7, 5, 8};
    for (int id : expectedOrder) assert(queue.pop().id == id);
    assert(queue.empty());

    // Test 6: a new Critical reported later jumps ahead of everything Serious,
    // but still waits behind the Critical that was reported before it.
    queue.push({20, Severity::Serious,  20});
    queue.push({21, Severity::Critical, 21});
    queue.push({22, Severity::Critical, 22});
    assert(queue.pop().id == 21);
    assert(queue.pop().id == 22);
    assert(queue.pop().id == 20);

    // Test 7: big test - 100000 random numbers must come out in non-increasing order
    BinaryHeap<int, BiggestFirst> big;
    std::srand(42);   // fixed seed: the same numbers every run
    for (int i = 0; i < 100000; ++i) big.push(std::rand() % 1000);
    int previous = big.pop();
    while (!big.empty()) {
        int current = big.pop();
        assert(current <= previous);
        previous = current;
    }

    std::cout << "All BinaryHeap tests passed\n";
    return 0;
}
