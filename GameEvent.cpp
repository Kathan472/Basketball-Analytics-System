// ============================================================
// GameEvent.cpp
//
// Base class for the game event inheritance hierarchy.
// Implements shared functionality for all game event types.
// The interesting behavior is in the subclasses.
//
// Implement the constructor, virtual destructor,
// and the four getter methods.
// ============================================================
#include "GameEvent.h"

// Constructor: store the four shared fields every event has
GameEvent::GameEvent(int team, double gtime, const string& clock, int pid) {
    // Assign each parameter to its matching member variable
    // team  → team_id
    // gtime → game_time
    // clock → real_clock
    // pid   → player_id
    team_id = team;
    game_time = gtime;
    real_clock = clock;
    player_id = pid;
}

// Virtual destructor.
// The body is intentionally empty — it just needs to exist so
// that deleting a subclass object via GameEvent* is safe.
GameEvent::~GameEvent() {
// Nothing to clean up here — subclasses handle their own data
}

// ── Getters ──────────────────────────────────────────────────
int GameEvent::getTeamID() const {
    return team_id;
}

double GameEvent::getGameTime() const {
    return game_time;
}

string GameEvent::getRealClock() const {
    return real_clock;
}

int GameEvent::getPlayerID() const {
    return player_id;
}