#include <cassert>
#include <iostream>
#include <stdexcept>
#include "domain/Incident.h"
#include "ds/BinaryHeap.h"
#include "ds/HashMap.h"

// The 10 demo emergencies, exactly as in the MySQL emergencies table.
struct DemoRow {
    int id;
    IncidentType type;
    int people;
    bool trapped;
    bool unconscious;
    int reporter;          // 0 = NULL in MySQL
    Severity expected;     // from the table in demo_script.md
};

const DemoRow kDemo[] = {
    {1,  IncidentType::Electrical, 2, false, false, 101, Severity::Moderate},
    {2,  IncidentType::Fire,       3, true,  false, 101, Severity::Critical},
    {3,  IncidentType::Collapse,   1, true,  true,  102, Severity::Critical},
    {4,  IncidentType::Medical,    1, false, true,  103, Severity::Serious},
    {5,  IncidentType::Fall,       1, false, false, 104, Severity::Minor},
    {6,  IncidentType::Vehicle,    2, false, false, 105, Severity::Moderate},
    {7,  IncidentType::Fire,       1, false, false, 106, Severity::Moderate},
    {8,  IncidentType::Medical,    1, false, false, 0,   Severity::Minor},
    {9,  IncidentType::Collapse,   2, true,  false, 107, Severity::Critical},
    {10, IncidentType::Medical,    2, false, true,  108, Severity::Serious},
};

int main() {
    // Test 1: the severity rule gives the expected severity for all 10 demo rows
    for (const DemoRow& row : kDemo) {
        Incident inc(row.id, row.type, 1, row.people, row.trapped, row.unconscious,
                     row.reporter, row.reporter != 0);
        assert(inc.severity() == row.expected);
    }

    // Test 2: boundary points of the rule
    assert(severityFromPoints(2) == Severity::Minor);
    assert(severityFromPoints(3) == Severity::Moderate);
    assert(severityFromPoints(4) == Severity::Serious);
    assert(severityFromPoints(5) == Severity::Serious);
    assert(severityFromPoints(6) == Severity::Critical);
    assert(severityFromPoints(8) == Severity::Critical);   // maximum possible: 3 + 2 + 2 + 1 = 8

    // Test 3: which team each incident type needs
    assert(specialistFor(IncidentType::Fire) == TeamType::Fire);
    assert(specialistFor(IncidentType::Medical) == TeamType::Medical);
    assert(specialistFor(IncidentType::Fall) == TeamType::Medical);
    assert(specialistFor(IncidentType::Electrical) == TeamType::Medical);
    assert(specialistFor(IncidentType::Collapse) == TeamType::General);
    assert(specialistFor(IncidentType::Vehicle) == TeamType::General);

    // Test 4: invalid data is rejected
    bool threw = false;
    try { Incident bad(11, IncidentType::Fall, 1, 0, false, false, 101, true); }   // 0 people
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    // Test 5: normal lifecycle  Pending -> Dispatched -> Resolved
    Incident fire(2, IncidentType::Fire, 5, 3, true, false, 101, true);
    assert(fire.status() == IncidentStatus::Pending && fire.assignedTeamId() == 0);
    fire.markDispatched(4);
    assert(fire.status() == IncidentStatus::Dispatched && fire.assignedTeamId() == 4);
    fire.markResolved();
    assert(fire.status() == IncidentStatus::Resolved);

    // Test 6: escalation lifecycle  Pending -> Escalated -> Dispatched
    Incident collapse(3, IncidentType::Collapse, 4, 1, true, true, 102, true);
    collapse.markEscalated();
    assert(collapse.status() == IncidentStatus::Escalated);
    collapse.markDispatched(7);   // a team became free later
    assert(collapse.status() == IncidentStatus::Dispatched);

    // Test 7: impossible status changes are rejected
    threw = false;
    try { fire.markDispatched(5); } catch (const std::logic_error&) { threw = true; }   // already resolved
    assert(threw);
    Incident fresh(5, IncidentType::Fall, 6, 1, false, false, 104, true);
    threw = false;
    try { fresh.markResolved(); } catch (const std::logic_error&) { threw = true; }     // never dispatched
    assert(threw);

    // Test 8: unknown reporter is accepted, but marked unverified (demo Step 3)
    Incident anon(8, IncidentType::Medical, 2, 1, false, false, Incident::kUnknownReporter, false);
    assert(!anon.reporterVerified() && anon.reporterId() == 0);

    // Test 9: the full system idea - incidents stored ONCE in a HashMap,
    // only (severity, id) in the priority queue. Dispatch order must match demo Step 2.
    HashMap<int, Incident> incidents;
    BinaryHeap<QueueEntry, IncidentPriority> queue;
    for (const DemoRow& row : kDemo) {
        Incident inc(row.id, row.type, 1, row.people, row.trapped, row.unconscious,
                     row.reporter, row.reporter != 0);
        queue.push(queueEntryFor(inc));
        incidents.insert(row.id, inc);
    }
    int expectedOrder[] = {2, 3, 9, 4, 10, 1, 6, 7, 5, 8};
    for (int id : expectedOrder) {
        QueueEntry next = queue.pop();
        assert(next.incidentId == id);
        Incident* inc = incidents.find(next.incidentId);
        assert(inc != nullptr);
        inc->markDispatched(1);   // status changes in the ONE stored copy
    }
    assert(incidents.find(2)->status() == IncidentStatus::Dispatched);

    std::cout << "All Incident tests passed\n";
    return 0;
}
