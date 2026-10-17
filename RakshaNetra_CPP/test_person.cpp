#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "domain/AccessControl.h"
#include "domain/Person.h"
#include "ds/HashMap.h"

int main() {
    const PPE fullPPE{true, true, true};
    const PPE noBoots{true, true, false};
    const PPE nothing{false, false, false};

    // Test 1: the factory creates the right derived class from the database category
    std::unique_ptr<Person> w = makePerson(Category::Worker, 101, "Rahul Sharma", "Electrician", 5, fullPPE, false);
    std::unique_ptr<Person> s = makePerson(Category::Staff, 104, "Vikas Rawat", "Supervisor", 2, fullPPE, false);
    std::unique_ptr<Person> c = makePerson(Category::Contractor, 120, "Kavita Joshi", "Survey", 3, fullPPE, false);

    // Test 2: polymorphism - the same call gives a different answer for each type
    assert(w->category() == Category::Worker && w->badge() == "Yellow");
    assert(s->category() == Category::Staff && s->badge() == "Blue");
    assert(c->category() == Category::Contractor && c->badge() == "Orange");
    assert(w->mayWorkAloneInRestrictedZone());
    assert(s->mayWorkAloneInRestrictedZone());
    assert(!c->mayWorkAloneInRestrictedZone());   // only Contractor overrides this

    // Test 3: encapsulation - invalid data is rejected
    bool threw = false;
    try { makePerson(Category::Worker, 0, "No ID", "Helper", 1, fullPPE, false); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { w->setZone(-3); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    assert(w->zoneId() == 5);   // unchanged after the failed update

    // Test 4: gate check - normal worker with full PPE
    AccessDecision d = checkGateEntry(*w, false, 0);
    assert(d.allowed && d.reason == "Access granted");

    // Test 5: gate check - missing boots (Step 7 of the demo)
    std::unique_ptr<Person> sahil = makePerson(Category::Worker, 109, "Sahil Kumar", "Mason", 3, noBoots, false);
    d = checkGateEntry(*sahil, false, 0);
    assert(!d.allowed && d.reason == "Missing PPE: boots");

    // Test 6: all PPE missing - every item is named
    std::unique_ptr<Person> bare = makePerson(Category::Worker, 130, "Test Person", "Helper", 3, nothing, false);
    d = checkGateEntry(*bare, false, 0);
    assert(d.reason == "Missing PPE: helmet vest boots");

    // Test 7: revoked ID card is checked FIRST (even with missing PPE)
    std::unique_ptr<Person> revoked = makePerson(Category::Worker, 111, "Revoked Person", "Driver", 1, noBoots, true);
    d = checkGateEntry(*revoked, false, 0);
    assert(!d.allowed && d.reason == "ID card revoked");

    // Test 8: contractor ALONE in a restricted zone (Tunnel Section) -> denied
    d = checkGateEntry(*c, true, 0);
    assert(!d.allowed && d.reason == "Cannot work alone in a restricted zone");

    // Test 9: contractor in a restricted zone WITH a worker already inside -> allowed
    d = checkGateEntry(*c, true, 1);
    assert(d.allowed);

    // Test 10: contractor in a normal zone -> allowed even alone
    d = checkGateEntry(*c, false, 0);
    assert(d.allowed);

    // Test 11: a worker may enter a restricted zone alone
    d = checkGateEntry(*w, true, 0);
    assert(d.allowed);

    // Test 12: storing different Person types together in your own HashMap
    // (this is how the worker registry will work)
    HashMap<int, std::unique_ptr<Person>> registry;
    // Read the ID into a variable FIRST. Writing registry.insert(w->id(), std::move(w))
    // is a bug: C++ may move w (making it null) before it calls w->id().
    int wId = w->id();
    int cId = c->id();
    registry.insert(wId, std::move(w));
    registry.insert(cId, std::move(c));
    std::unique_ptr<Person>* found = registry.find(120);
    assert(found != nullptr && (*found)->category() == Category::Contractor);
    assert(registry.find(999) == nullptr);   // unknown ID -> unverified

    std::cout << "All Person tests passed\n";
    return 0;
}
