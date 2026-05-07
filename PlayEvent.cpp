// ============================================================
// PlayEvent.cpp
//
// play_type values: "2PT", "3PT", "FT", "REBOUND", "FOUL"
// outcome values:   "MADE", "MISS", "OFF", "DEF", "SHOOTING"
//
// Important: FOUL records store the FOULED player's ID (the
// shooter), not the fouling player. This lets Game look up
// the shooter's position to determine the foul zone.
// ============================================================
#include "PlayEvent.h"
#include <iostream>
#include <iomanip>
using namespace std;

// ── Constructors ─────────────────────────────────────────────

// Initialize event with default values
PlayEvent::PlayEvent() {
    play_id = 0;
    player_id = 0;
    play_type = "";
    outcome = "";
    points = 0;
    game_time = 0.0;
    real_clock = "";
}

// Parameterized constructor initializes all fields to the given values.
PlayEvent::PlayEvent(int pid, int player, const string& type, const string& out, int pts, double gtime, const string& clock) {
    play_id = pid;
    player_id = player;
    play_type = type;
    outcome = out;
    points = pts;
    game_time = gtime;
    real_clock = clock;
}

// ── Getters ──────────────────────────────────────────────────

int PlayEvent::getPlayID() const {
    return play_id;
}

int PlayEvent::getPlayerID() const {
    return player_id;
}

string PlayEvent::getPlayType() const {
    return play_type;
}

string PlayEvent::getOutcome() const {
    return outcome;
}

int PlayEvent::getPoints() const {
    return points;
}

double PlayEvent::getGameTime() const {
    return game_time;
}

string PlayEvent::getRealClock() const {
    return real_clock;
}

// ── Setters ──────────────────────────────────────────────────

void PlayEvent::setPlayID(int id) { 
    play_id = id; 
}
void PlayEvent::setPlayerID(int id) { 
    player_id = id; 
}
void PlayEvent::setPlayType(const string& t) { 
    play_type = t; 
}
void PlayEvent::setOutcome(const string& o) {
    outcome = o; 
}
void PlayEvent::setPoints(int pts) {
    points = pts; 
}
void PlayEvent::setGameTime(double t) { 
    game_time = t; 
}
void PlayEvent::setRealClock(const string& c) { 
    real_clock = c; 
}

// ── Display ──────────────────────────────────────────────────

// Print a one-line play summary.
// Expected format:
//   Player 1: 2PT at 11:45 (0.5s) - MISS (0 pts)
void PlayEvent::display() const {
    cout << "Player " << player_id << ": " << play_type
         << " at " << real_clock << " (" << fixed << setprecision(1)
         << game_time << "s) - " << outcome
         << " (" << points << " pts)" << endl;
}