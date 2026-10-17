#pragma once
#include <string>
#include "domain/Person.h"

// The result of a gate check. A denial is a NORMAL outcome (not an error),
// so it is returned as a value instead of being thrown as an exception.
struct AccessDecision {
    bool allowed = false;
    std::string reason;
};

// Gate check-in rules, checked in this order:
//   1. ID card revoked                -> DENIED
//   2. any PPE item missing           -> DENIED (names the missing items)
//   3. restricted zone, person may not work alone there (contractor),
//      and no Worker/Staff is already inside -> DENIED
//   otherwise                         -> ALLOWED
//
// zoneRestricted        : the "restricted" column of the zones table
// supervisorsInZone     : how many Workers/Staff are currently checked in to that zone
//                         (the system counts this from the check-in records)
inline AccessDecision checkGateEntry(const Person& person, bool zoneRestricted,
                                     int supervisorsInZone) {
    if (person.idRevoked()) {
        return {false, "ID card revoked"};
    }

    std::string missing;
    const PPE& ppe = person.ppe();
    if (!ppe.helmet) missing += "helmet ";
    if (!ppe.vest)   missing += "vest ";
    if (!ppe.boots)  missing += "boots ";
    if (!missing.empty()) {
        missing.pop_back();   // remove the last space
        return {false, "Missing PPE: " + missing};
    }

    // Polymorphism: we do not check "is this a Contractor?". We ask the object,
    // and the Contractor class answers false. Adding a new type later (e.g. Visitor)
    // needs no change here.
    if (zoneRestricted && !person.mayWorkAloneInRestrictedZone() && supervisorsInZone == 0) {
        return {false, "Cannot work alone in a restricted zone"};
    }

    return {true, "Access granted"};
}
