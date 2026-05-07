// ============================================================
// ReboundEvent.cpp
//
// Represents a rebound (offensive or defensive).
// Inherits from GameEvent.
// getPoints() always returns 0.
// ============================================================
#include "ReboundEvent.h"
#include <sstream>
using namespace std;

// Constructor
// type is "OFF" (offensive) or "DEF" (defensive)
ReboundEvent::ReboundEvent(int team, double gtime, const string& clock, int pid, const string& type) : GameEvent(team, gtime, clock, pid) {
    rebound_type = type;
}

ReboundEvent::~ReboundEvent() {}

// ── Getter ───────────────────────────────────────────────────

string ReboundEvent::getReboundType() const {
    return rebound_type;
}

// ── GameEvent interface ───────────────────────────────────────

// Rebounds do not contribute scoring
int ReboundEvent::getPoints() const {
    return 0;
}

// Return a human-readable description.
// Expected format:
//   OFF REBOUND | 11:42
//   DEF REBOUND | 11:38
string ReboundEvent::describe() const {
    // Build and return the description string above.
    // Use ostringstream oss; ... return oss.str();
    // Include rebound_type and real_clock.

    // ostringstream oss used to build description
    ostringstream oss;

    // Build the string in the format above, using rebound_type and real_clock
    oss << rebound_type << " REBOUND | " << getRealClock();

    // return the string description
    return oss.str();
}

// Return the event type string used by the game log.
string ReboundEvent::getEventType() const {
    return "REBOUND";
}