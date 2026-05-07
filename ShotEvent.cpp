// ============================================================
// ShotEvent.cpp
//
// Represents a shot attempt (2PT, 3PT, or FT).
// Inherits from GameEvent.
//
// ── Distance → Zone → Points ─────────────────────────────────
//    0 –  5 ft  →  1 pt   (layup / dunk zone)
//    5 – 15 ft  →  2 pts  (short mid-range)
//   15 – 23 ft  →  3 pts  (long mid-range)
//   23 – 30 ft  →  4 pts  (beyond arc)
//   30+ ft      →  5 pts  (half-court)
//
// ── Free Throws ───────────────────────────────────────────────
//   FTs are always 1 pt if made. Distance is not applicable.
//   Use the FT constructor; zone_value is set to 1 automatically.
// ============================================================
#include "ShotEvent.h"
#include <sstream>
#include <iomanip>
using namespace std;

// Map a distance in feet to a zone value (1-5).
// Maps shot distance to scoring zone.
int ShotEvent::computeZone(double dist) {
    // Return the correct zone value based on dist.
    // if dist <  5.0  → return 1
    // if dist < 15.0  → return 2
    // if dist < 23.0  → return 3
    // if dist < 30.0  → return 4
    // otherwise       → return 5
    // Distance-based scoring zones
    if (dist < 5.0) {
        return 1;
    }
    if (dist < 15.0) {
        return 2;
    }
    if (dist < 23.0) {
        return 3;
    }
    if (dist < 30.0) {
        return 4;
    }
    return 5;
}
// ── Field goal constructor ────────────────────────────────────
// Used for 2PT and 3PT shots. Distance is already computed by Game.

ShotEvent::ShotEvent(int team, double gtime, const string& clock, int pid, const string& type, const string& out, double dist) : GameEvent(team, gtime, clock, pid) {
    shot_type = type;
    outcome = out;
    distance = dist;
    zone_value = computeZone(dist);
}

// ── Free throw constructor ────────────────────────────────────
// Used for FT shots. Distance does not apply; zone_value is always 1.

ShotEvent::ShotEvent(int team, double gtime, const string& clock, int pid, const string& out) : GameEvent(team, gtime, clock, pid) {
    // Set shot_type = "FT", outcome = out,
    // distance = 0.0, zone_value = 1
    shot_type = "FT";
    outcome = out;
    distance = 0.0;
    zone_value = 1;
}

ShotEvent::~ShotEvent() {}

// ── Getters ──────────────────────────────────────────────────

string ShotEvent::getShotType()  const { 
    return shot_type; 
}
string ShotEvent::getOutcome()   const { 
    return outcome; 
}
double ShotEvent::getDistance()  const {  
    return distance; 
}
int    ShotEvent::getZoneValue() const {  
    return zone_value; 
}

// ── GameEvent interface ────────────────────────────────────────

// Return the point value of this shot.
// Rule: if outcome is "MADE" → return zone_value
// if outcome is "MISS" → return 0
int ShotEvent::getPoints() const {
    // Check outcome and return zone_value or 0
    if (outcome == "MADE") {
        return zone_value;
    }
    return 0;
}

// Used by Option 22 (game log) via virtual dispatch.
//
// Expected format for field goals:
// 2PT from 4.7ft (1pt zone) | MADE | 1 pts | 11:45
//
// Expected format for free throws:
// FT | MADE | 1 pt | 11:19
string ShotEvent::describe() const {

    // ostringstream oss used to build description;
    ostringstream oss;

    // If shot type is free throw (FT), use the free throw format.
    if (shot_type == "FT") {
        oss << "FT | " << outcome << " | " << getPoints() << " pt | " << getRealClock();
    } 
    
    // /if it's a field goal (2PT or 3PT), use the field goal format.
    else {
       oss << shot_type << " from " << fixed << setprecision(1) << distance << "ft (" << zone_value << "pt zone) | " << outcome << " | " << getPoints() << " pts | " << getRealClock();
    }

    // Return the built description string.
    return oss.str();
}

// Return the event type string used by the game log.
// Always returns "SHOT" for all shot types.
string ShotEvent::getEventType() const {
    return "SHOT";
}