#pragma once
#include <stdexcept>
#include <string>
#include "core/Enums.h"

// ---------------------------------------------------------------------------
// Severity rule (team decision - also written in demo_script.md)
//
//   Base points:  Fire or Collapse = 3,  any other type = 2
//   +2 if someone is trapped
//   +2 if someone is unconscious
//   +1 if more than one person is affected
//
//   6 or more -> Critical,  4-5 -> Serious,  3 -> Moderate,  2 -> Minor
//
// It is a separate function so it can be tested on its own and changed in ONE place.
// ---------------------------------------------------------------------------
inline int severityPoints(IncidentType type, int peopleAffected, bool trapped, bool unconscious) {
    int points = (type == IncidentType::Fire || type == IncidentType::Collapse) ? 3 : 2;
    if (trapped) points += 2;
    if (unconscious) points += 2;
    if (peopleAffected > 1) points += 1;
    return points;
}

inline Severity severityFromPoints(int points) {
    if (points >= 6) return Severity::Critical;
    if (points >= 4) return Severity::Serious;
    if (points == 3) return Severity::Moderate;
    return Severity::Minor;
}

// ---------------------------------------------------------------------------
// Which kind of team each incident type needs (team decision):
//   Fire                        -> Fire team
//   Medical, Fall, Electrical   -> Medical team (injury treatment)
//   Collapse, Vehicle           -> General rescue team (lifting, freeing people)
// If the specialist team is busy, the dispatcher falls back to a General team.
// ---------------------------------------------------------------------------
inline TeamType specialistFor(IncidentType type) {
    switch (type) {
        case IncidentType::Fire:       return TeamType::Fire;
        case IncidentType::Medical:
        case IncidentType::Fall:
        case IncidentType::Electrical: return TeamType::Medical;
        case IncidentType::Collapse:
        case IncidentType::Vehicle:    return TeamType::General;
    }
    return TeamType::General;
}

// ---------------------------------------------------------------------------
// Incident: one row of the MySQL "emergencies" table.
//
// The id is the AUTO_INCREMENT emergency_id, so a smaller id = reported earlier.
// It is also used as the tie-break when two incidents have the same severity.
//
// Status can only change along these arrows (anything else throws):
//
//   Pending ----> Dispatched ----> Resolved
//      |              ^
//      v              |
//   Escalated --------+      (escalated incidents are retried when a team is free)
// ---------------------------------------------------------------------------
class Incident {
public:
    static constexpr int kUnknownReporter = 0;   // MySQL worker_id was NULL

    Incident(int id, IncidentType type, int zoneId, int peopleAffected,
             bool trapped, bool unconscious, int reporterId, bool reporterVerified)
        : id_(id), type_(type), zoneId_(zoneId), peopleAffected_(peopleAffected),
          trapped_(trapped), unconscious_(unconscious),
          reporterId_(reporterId), reporterVerified_(reporterVerified) {
        if (id_ <= 0) throw std::invalid_argument("Incident ID must be positive");
        if (peopleAffected_ < 1) throw std::invalid_argument("At least one person must be affected");
        // Severity is calculated ONCE, from the facts. It is never typed in by hand.
        severity_ = severityFromPoints(severityPoints(type_, peopleAffected_, trapped_, unconscious_));
    }

    // ---- Getters ----
    int id() const { return id_; }
    IncidentType type() const { return type_; }
    int zoneId() const { return zoneId_; }
    int peopleAffected() const { return peopleAffected_; }
    bool trapped() const { return trapped_; }
    bool unconscious() const { return unconscious_; }
    int reporterId() const { return reporterId_; }
    bool reporterVerified() const { return reporterVerified_; }
    Severity severity() const { return severity_; }
    IncidentStatus status() const { return status_; }
    int assignedTeamId() const { return assignedTeamId_; }
    TeamType specialistNeeded() const { return specialistFor(type_); }

    // ---- Status changes (only the allowed arrows) ----
    void markDispatched(int teamId) {
        if (status_ != IncidentStatus::Pending && status_ != IncidentStatus::Escalated)
            throw std::logic_error("Only a Pending or Escalated incident can be dispatched");
        if (teamId <= 0) throw std::invalid_argument("Team ID must be positive");
        status_ = IncidentStatus::Dispatched;
        assignedTeamId_ = teamId;
    }

    void markEscalated() {
        if (status_ != IncidentStatus::Pending)
            throw std::logic_error("Only a Pending incident can be escalated");
        status_ = IncidentStatus::Escalated;
    }

    void markResolved() {
        if (status_ != IncidentStatus::Dispatched)
            throw std::logic_error("Only a Dispatched incident can be resolved");
        status_ = IncidentStatus::Resolved;
    }

private:
    int id_;
    IncidentType type_;
    int zoneId_;
    int peopleAffected_;
    bool trapped_;
    bool unconscious_;
    int reporterId_;
    bool reporterVerified_;
    Severity severity_ = Severity::Minor;
    IncidentStatus status_ = IncidentStatus::Pending;
    int assignedTeamId_ = 0;   // 0 = no team yet
};

// ---------------------------------------------------------------------------
// What goes into the emergency priority queue: only the severity and the ID.
// The full Incident stays in ONE place (a HashMap by ID), so its status is
// never out of date in a second copy.
// ---------------------------------------------------------------------------
struct QueueEntry {
    Severity severity = Severity::Minor;
    int incidentId = 0;
};

// Priority rule for BinaryHeap: less(a, b) = "a should be handled AFTER b".
//   1. higher severity first
//   2. same severity -> smaller ID (reported earlier) first
struct IncidentPriority {
    bool operator()(const QueueEntry& a, const QueueEntry& b) const {
        if (a.severity != b.severity) return a.severity < b.severity;
        return a.incidentId > b.incidentId;
    }
};

inline QueueEntry queueEntryFor(const Incident& inc) {
    return {inc.severity(), inc.id()};
}
