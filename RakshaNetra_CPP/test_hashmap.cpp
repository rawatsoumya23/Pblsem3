#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "ds/HashMap.h"

// A simplified worker record, like a row from the workers table.
struct Worker {
    std::string name;
    std::string role;
    int zoneId = 0;
};

int main() {
    // Test 1: a new map is empty
    HashMap<int, Worker> workers;
    assert(workers.empty() && workers.size() == 0);
    assert(workers.find(101) == nullptr);

    // Test 2: insert and find (like your workers table)
    assert(workers.insert(101, {"Rahul Sharma", "Electrician", 5}));
    assert(workers.insert(102, {"Aman Kumar", "Mason", 6}));
    assert(workers.insert(104, {"Vikas Rawat", "Supervisor", 2}));
    assert(workers.size() == 3);
    Worker* w = workers.find(102);
    assert(w != nullptr && w->name == "Aman Kumar");

    // Test 3: the pointer lets us UPDATE the stored record
    w->zoneId = 3;
    assert(workers.find(102)->zoneId == 3);

    // Test 4: a duplicate ID is rejected and the original is NOT overwritten
    assert(!workers.insert(101, {"Someone Else", "Helper", 1}));
    assert(workers.find(101)->name == "Rahul Sharma");
    assert(workers.size() == 3);

    // Test 5: unknown ID -> nullptr (this is the "unverified reporter" case)
    assert(workers.find(999) == nullptr);
    assert(!workers.contains(999));

    // Test 6: collisions. With 8 buckets, keys 1, 9 and 17 all land in bucket 1.
    // Erase the middle one, then the others, and check the rest stay findable.
    HashMap<int, int> m;
    assert(m.bucketCount() == 8);
    m.insert(1, 100);
    m.insert(9, 900);
    m.insert(17, 1700);
    assert(m.erase(9));                       // remove from the middle of the chain
    assert(m.find(9) == nullptr);
    assert(*m.find(1) == 100 && *m.find(17) == 1700);
    assert(m.erase(17));                      // remove the first in the chain
    assert(m.erase(1));                       // remove the last remaining
    assert(m.empty());
    assert(!m.erase(1));                      // erasing something absent returns false

    // Test 7: rehash keeps every key, and old pointers stay valid
    HashMap<int, int> grow;
    grow.insert(5, 50);
    int* fivePtr = grow.find(5);
    for (int i = 1000; i < 11000; ++i) grow.insert(i, i * 2);   // forces many rehashes
    assert(grow.bucketCount() >= grow.size());                  // load factor <= 1
    assert(fivePtr == grow.find(5) && *fivePtr == 50);           // same entry, not a copy
    bool allFound = true;
    for (int i = 1000; i < 11000; ++i) {
        const int* v = grow.find(i);
        if (v == nullptr || *v != i * 2) allFound = false;
    }
    assert(allFound);

    // Test 8: zero and negative keys work
    HashMap<int, std::string> odd;
    odd.insert(0, "zero");
    odd.insert(-7, "minus seven");
    assert(*odd.find(0) == "zero" && *odd.find(-7) == "minus seven");

    // Test 9: string keys (e.g. zone name -> zone ID)
    HashMap<std::string, int> zones;
    zones.insert("Main Gate", 1);
    zones.insert("Tunnel Section", 5);
    assert(*zones.find("Tunnel Section") == 5);
    assert(zones.find("tunnel section") == nullptr);   // exact spelling required

    // Test 10: values that cannot be copied (needed later for Person objects)
    HashMap<int, std::unique_ptr<std::string>> owners;
    owners.insert(1, std::make_unique<std::string>("Fire Rescue Team 1"));
    assert(**owners.find(1) == "Fire Rescue Team 1");

    // Test 11: forEach visits every entry exactly once
    int count = 0;
    workers.forEach([&count](int, const Worker&) { ++count; });
    assert(count == 3);

    // Test 12: big test - 100000 IDs
    HashMap<int, int> big;
    for (int i = 0; i < 100000; ++i) assert(big.insert(i, i));
    assert(big.size() == 100000);
    for (int i = 0; i < 100000; i += 2) assert(big.erase(i));   // remove all even IDs
    assert(big.size() == 50000);
    assert(big.find(4) == nullptr && *big.find(5) == 5);

    std::cout << "All HashMap tests passed\n";
    return 0;
}
