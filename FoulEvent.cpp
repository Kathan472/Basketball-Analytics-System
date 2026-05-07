// ============================================================
// FoulEvent.cpp
//
// Represents a shooting foul. Inherits from GameEvent.
//
// How it works:
//   1. The fouled player was shooting from some distance.
//   2. That distance maps to a zone value (1-5), same as ShotEvent.
//   3. The zone value determines how many free throws are awarded.
//   4. Game::buildGameEvents() counts the actual FT records that
//      follow this foul and calls setFTMade() to record them.
//   5. FoulEvent::getPoints() always returns 0 - the points from the
//      free throws are recorded separately through the ShotEvent records.
//
// ── Zone → Free Throws Awarded ───────────────────────────────
//   Foul in 1pt zone → 1 FT  awarded
//   Foul in 2pt zone → 2 FTs awarded
//   Foul in 3pt zone → 3 FTs awarded
//   Foul in 4pt zone → 4 FTs awarded
//   Foul in 5pt zone → 5 FTs awarded
// ============================================================
#include "FoulEvent.h"
#include <sstream>
#include <iomanip>
using namespace std;

int FoulEvent::zoneFromDist(double dist)
{
    //   Return zone 1-5 based on dist
    //   dist <  5.0 → 1
    //   dist < 15.0 → 2
    //   dist < 23.0 → 3
    //   dist < 30.0 → 4
    //   otherwise   → 5
    
    // Determine foul zone from shot distance
    if (dist < 5.0) {
        return 1;
    } 
    else if (dist < 15.0) {
        return 2;
    } 
    else if (dist < 23.0) {
        return 3;
    } 
    else if (dist < 30.0) {
        return 4;
    } 
    else {
        return 5;
    }
}

// Constructor
// dist is the distance of the FOULED player from the basket.
// Use zoneFromDist() to compute foul_zone.
// ft_awarded equals foul_zone (one free throw per zone level).
// ft_made starts at 0 — it will be set later by setFTMade().
FoulEvent::FoulEvent(int team, double gtime, const string& clock, int pid, double dist) : GameEvent(team, gtime, clock, pid) {
    // Set distance, compute foul_zone via zoneFromDist(),
    // set ft_awarded = foul_zone, set ft_made = 0
    distance = dist;
    foul_zone = zoneFromDist(dist);
    ft_awarded = foul_zone;
    ft_made = 0;
}

FoulEvent::~FoulEvent() {}

// ── Getters ──────────────────────────────────────────────────

int FoulEvent::getFoulZone() const { 
    return foul_zone;   
}
int FoulEvent::getFTAwarded() const { 
    return ft_awarded;   
}
int FoulEvent::getFTMade() const { 
    return ft_made;   
}
double FoulEvent::getDistance() const {  
    return distance; 
}

// Called by Game after counting the FT records that follow this foul
void FoulEvent::setFTMade(int made) {
    // Assign made to ft_made
    ft_made = made;
}

// ── GameEvent interface ───────────────────────────────────────

// FoulEvent::getPoints() returns 0.
// Points from free throws are credited through the ShotEvent
// records that follow this foul - returning them here too
// would double-count them in the simulation total.
int FoulEvent::getPoints() const {
    return 0;
}

// Return a human-readable description of this foul.
// Used by Option 22 (game log) via virtual dispatch.
// Format:
// SHOOTING foul in 2pt zone -> 2 free throws awarded (2 made) | 11:21
string FoulEvent::describe() const {
    // LEVEL B: Build and return the description string above.
    // Use ostringstream oss; ... return oss.str();
    // Fields to include: foul_zone, ft_awarded, ft_made, real_clock

    ostringstream oss; // oss is used to build the description string

    // Build the description string
    oss << "SHOOTING foul zone " << foul_zone << " -> " << ft_awarded << " FTs awarded (" << ft_made << " made) | " << real_clock;

    // return the built description string
    return oss.str();
}

// Return the event type string used by the game log.
string FoulEvent::getEventType() const {
    return "FOUL";
}